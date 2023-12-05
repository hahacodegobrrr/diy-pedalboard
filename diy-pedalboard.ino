//arduino libraries
#include <Audio.h> //for processing audio
#include <Wire.h> //communication protocol (i2c)
#include <SPI.h> //communication protocol
#include <Bounce.h>

//c libraries
#include <stdint.h> //extended int types
#include <math.h> //extended math operations

const uint8_t MESSAGE_SIZE = 32; //in bytes
const float FFT_BIN_SIZE = 43; //hz

const float FFT_THRESHOLD = 0.0003; //anything below this is noise


const uint16_t SCREEN_UPDATE_PERIOD = 200; //in ms
const uint16_t DELAY_ENVELOPE_ON_PERIOD = 100;
const uint16_t DELAY_ENVELOPE_OFF_OFFSET = 10;

//used for tuner
struct TunerNote {
  uint8_t note;
  float error;
};
const float ln2 = 0.69314718f;
const float FREQ_A4 = 440;
uint8_t noteTable[] = {'A','a','B','C','c','D','d','E','F','f','G','g'};
struct TunerNote tunerNote;
uint8_t tunerMessage[MESSAGE_SIZE] = {'T','U','N','E','R',' ',' ',' ','N','O','T','E',':',' '};

//used for FSM
struct State {
  uint8_t id;
  uint16_t knob1;
  uint16_t knob2;
  uint16_t knob3;
  struct State* nextState;
};

//switch depending on which one we're using
const uint8_t input = AUDIO_INPUT_LINEIN;
// const uint8_t input = AUDIO_INPUT_MIC;

//for tracking timing
uint64_t lastScreenUpdate;
uint64_t lastDelayEnvelopeOn;
uint64_t lastDelayEnvelopeOff;

//audio shit
AudioInputI2S audioInput;   //audio shield line/mic in
AudioOutputI2S audioOutput;  //audio shield line out
AudioControlSGTL5000 audioShield;

//delay
AudioEffectEnvelope delayEnvelope;
AudioEffectDelay delayEffect;
AudioMixer4 delayMixer;

AudioAnalyzeNoteFrequency noteFreq;

//a module cannot have multiple inputs -> send to a mixer
//a module can have multiple outputs
AudioConnection patchCordTuner(audioInput, 0, noteFreq, 0);
AudioConnection in2de(audioInput, delayEnvelope);
AudioConnection de2delay(delayEnvelope, delayEffect);
AudioConnection de2out(delayEnvelope, 0, audioOutput, 0);
AudioConnection delay2mixer(delayEffect, 0, delayMixer, 0);
AudioConnection delay2mixer1(delayEffect, 1, delayMixer, 1);
AudioConnection delay2mixer2(delayEffect, 2, delayMixer, 2);
AudioConnection in2mixer3(audioInput, 0, delayMixer, 3);
AudioConnection in2mixer(audioInput, 0, delayMixer, 0);
AudioConnection mixer2out(delayMixer, 0, audioOutput, 1);

//controlled by knobs
uint16_t delayPeriod = 500;

struct State* currentState;

//buffer for message to be displayed on screen
uint8_t screenMessage[MESSAGE_SIZE];

void setup(){
  AudioMemory(1800); //change later (probably needs to increase)
  audioShield.enable();
  audioShield.inputSelect(input);
  audioShield.volume(0.5);

  noteFreq.begin(0.15);
  
  setDelayPeriod(delayPeriod);

  currentState = getNextState(0);

  Serial1.begin(9600);
  lastScreenUpdate = millis();
  lastDelayEnvelopeOn = lastScreenUpdate;
  lastDelayEnvelopeOff = lastScreenUpdate + DELAY_ENVELOPE_OFF_OFFSET;
}

void loop(){
  uint64_t now = millis();
  //screen
  if (lastScreenUpdate + SCREEN_UPDATE_PERIOD <= now) {
    lastScreenUpdate = now;
    if (currentState->id == 0 && noteFreq.available()) { //tuner mode
      uint8_t* msg = generateTunerDisplayMessage(noteFreq.read());
      sendMessageToScreen(msg, MESSAGE_SIZE);
    }
  }

  if (lastDelayEnvelopeOn + DELAY_ENVELOPE_ON_PERIOD <= now) {
    lastDelayEnvelopeOn = now;
    delayEnvelope.noteOn();
  }
  if (lastDelayEnvelopeOff + DELAY_ENVELOPE_ON_PERIOD <= now) {
    lastDelayEnvelopeOff = now;
    delayEnvelope.noteOff();
  }
}

/** Sends a message through Serial to Arduino
 *  p_text -> pointer to a char array containing the message
 *  length -> the size of the char array
 */
void sendMessageToScreen(uint8_t* p_text, uint8_t length) {
  Serial1.write(0); //signal beginning of message
  Serial1.write(p_text, length);
}

//TUNER
/** Converts a frequency into the corresponding note
 * frequency -> frequency, in hertz
 */
void getNote(float frequency) {
  float logStuff = log(frequency / FREQ_A4) / ln2;
  float freqTo12Tone;
  if (logStuff < 0)
    freqTo12Tone = 12 - ((float)(fmod(-logStuff, 1)) * 12);
  else
    freqTo12Tone = ((float)(fmod(logStuff, 1)) * 12);

  float error = fmod(freqTo12Tone, 1);
  uint16_t note = (uint16_t)freqTo12Tone;
  if (error >= 0.5) {
    error -= 1; //wrap around to next note's error
    note = (note + 1) % 12;
  }

  tunerNote.note = note;
  tunerNote.error = error;  
}

/** Generates a screen message for the tuner based on a frequency
 * frequency -> frequency, in hertz
 * returns a pointer to the message
 */
uint8_t* generateTunerDisplayMessage(float frequency) {
  //message indices 0-13 have already been written to
  getNote(frequency);
  if (noteTable[tunerNote.note] >= 'a') {
    tunerMessage[14] = noteTable[tunerNote.note] - 32;
    tunerMessage[15] = '#';
  } else {
    tunerMessage[14] = noteTable[tunerNote.note];
    tunerMessage[15] = ' ';
  }
  int i;
  for (i = 0; i < MESSAGE_SIZE / 2; i++) {
    float transI = (i - 7.5) * 0.5 / 7.5; //transforms (0-15) to (-0.5-0.5)
    if (tunerNote.error > 0)
      tunerMessage[i + MESSAGE_SIZE / 2] = (transI < tunerNote.error && transI > 0)? '-' : ' ';
    else
      tunerMessage[i + MESSAGE_SIZE / 2] = (transI > tunerNote.error && transI < 0)? '-' : ' ';
  }

  return &tunerMessage[0];
}

//FSM
//tuner, chorus, delay, freeverb
struct State stateTable[] = {
  {.id = 0, .knob1 = 0, .knob2 = 0, .knob3 = 0, .nextState = &stateTable[1]},
  {.id = 1, .knob1 = 0, .knob2 = 0, .knob3 = 0, .nextState = &stateTable[2]},
  {.id = 2, .knob1 = 0, .knob2 = 0, .knob3 = 0, .nextState = &stateTable[3]},
  {.id = 3, .knob1 = 0, .knob2 = 0, .knob3 = 0, .nextState = &stateTable[0]}
};

/** Get the next state in the FSM
 * currentState -> pointer to the current state
 * returns a pointer to the next state
 */
struct State* getNextState(struct State* currentState) {
  if (currentState == 0)
    return &stateTable[0];
  return (*currentState).nextState;
}

void setDelayPeriod(uint8_t newDP) {
  delayPeriod = newDP;
  delayEffect.delay(0, delayPeriod);
  delayEffect.delay(1, delayPeriod * 2);
  delayEffect.delay(2, delayPeriod * 3);
}
