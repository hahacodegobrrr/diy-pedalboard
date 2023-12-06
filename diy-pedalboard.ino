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
uint8_t tunerMessage[MESSAGE_SIZE]  = {'T','U','N','E','R',' ',' ',' ','N','O','T','E',':',' '};
uint8_t delayMessage[MESSAGE_SIZE]  = {'D','E','L','A','Y',':',' ','-','-','-',' ','M','S'};
uint8_t reverbMessage[MESSAGE_SIZE] = {'R','E','V','E','R','B',':',' ','-','-','-',' ','%'};
//used for FSM
struct State {
  uint8_t id;
  uint16_t knob;
  struct State* nextState;
};
//tuner, delay, freeverb
struct State stateTable[] = {
  {.id = 0, .knob = 0, .nextState = &stateTable[1]},
  {.id = 1, .knob = 0, .nextState = &stateTable[2]},
  {.id = 2, .knob = 0, .nextState = &stateTable[0]}
  // {.id = 3, .knob = 0, .nextState = &stateTable[0]}
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
AudioMixer4 delayMasterMixer;
AudioEffectFreeverb reverbEffect;
AudioMixer4 reverbMixer;

AudioMixer4 masterMixer;

AudioAnalyzeNoteFrequency noteFreq;

//a module cannot have multiple inputs -> send to a mixer
//a module can have multiple outputs
AudioConnection patchCordTuner(audioInput, 0, noteFreq, 0);

AudioConnection in2de(audioInput, delayEnvelope);
AudioConnection de2delay(delayEnvelope, delayEffect);
AudioConnection de2out(delayEnvelope, 0, delayMasterMixer, 0);
AudioConnection delay2dm(delayEffect, 0, delayMixer, 0);
AudioConnection delay2dm1(delayEffect, 1, delayMixer, 1);
AudioConnection delay2dm2(delayEffect, 2, delayMixer, 2);
AudioConnection in2dm3(audioInput, 0, delayMixer, 3);
AudioConnection in2dm(audioInput, 0, delayMixer, 0);
AudioConnection mixer2dmm(delayMixer, 0, delayMasterMixer, 1);
AudioConnection dmm2mm(delayMasterMixer, 0, masterMixer, 0);

AudioConnection in2re(audioInput, reverbEffect);
AudioConnection in2rm0(audioInput, 0, reverbMixer, 1);
AudioConnection re2rm1(reverbEffect, 0, reverbMixer, 0);
AudioConnection rm2mm(reverbMixer, 0, masterMixer, 1);

AudioConnection mm2out(masterMixer, audioOutput);

//controlled by knobs
uint16_t delayPeriod = 500;

struct State* currentState;

//buffer for message to be displayed on screen
uint8_t screenMessage[MESSAGE_SIZE];

//bit 0 -> delay
//bit 1 -> reverb
uint8_t effectsFlags;

const uint8_t cycleButtonPin = 16;
Bounce cycleButton = Bounce(cycleButtonPin, 100);
const uint8_t saveButtonPin = 22;
Bounce saveButton = Bounce(saveButtonPin, 100);

//knob runs from 521 to 1023
const uint8_t knobPin = 14;

void setup(){
  AudioMemory(1800); //change later (probably needs to increase)
  audioShield.enable();
  audioShield.inputSelect(input);
  audioShield.volume(0.5);

  int i;
  for (i = 14; i < MESSAGE_SIZE; i++)
    tunerMessage[i] = ' ';
  for (i = 13; i < MESSAGE_SIZE; i++)
    delayMessage[i] = ' ';
  for (i = 13; i < MESSAGE_SIZE; i++)
    reverbMessage[i] = ' ';

  noteFreq.begin(0.15);
  
  effectsFlags = 0;
  setDelayPeriod(100);

  pinMode(cycleButtonPin, INPUT);
  pinMode(saveButtonPin, INPUT);
  pinMode(knobPin, INPUT);

  currentState = getNextState(0);

  Serial1.begin(9600);
  lastScreenUpdate = millis();
  lastDelayEnvelopeOn = lastScreenUpdate;
  lastDelayEnvelopeOff = lastScreenUpdate + DELAY_ENVELOPE_OFF_OFFSET;
}

void loop(){
  uint64_t now = millis();
  if (cycleButton.update()) {
    if (cycleButton.risingEdge()) {
      currentState = getNextState(currentState);
      if (currentState->id == 1)
        effectsFlags = 0xFF;
    }
  }

  //tuner
  if (currentState->id == 0) {
    effectsFlags = 0;
    //screen
    if (lastScreenUpdate + SCREEN_UPDATE_PERIOD <= now && noteFreq.available()) {
      lastScreenUpdate = now;
      uint8_t* msg = generateTunerDisplayMessage(noteFreq.read());
      sendMessageToScreen(msg, MESSAGE_SIZE);
    }
  }

  //delay
  if (currentState->id == 1) {
    if (saveButton.update()) {
      if (saveButton.risingEdge()) {
        uint16_t knob = analogRead(knobPin);
        currentState->knob = knob;
        setDelayPeriod(knobToDelayTime(knob));
      }
    }
    //screen
    if (lastScreenUpdate + SCREEN_UPDATE_PERIOD <= now) {
      lastScreenUpdate = now;
      uint16_t knob = analogRead(knobPin);
      uint16_t msDelay = knobToDelayTime(knob);
      //789
      delayMessage[7] = (msDelay / 100) + '0';
      delayMessage[8] = (msDelay / 10) % 10 + '0';
      delayMessage[9] = msDelay % 10 + '0';
      sendMessageToScreen(&delayMessage[0], MESSAGE_SIZE);
    }
  }

  //reverb
  if (currentState->id == 2) {
    if (saveButton.update()) {
      if (saveButton.risingEdge()) {
        uint16_t knob = analogRead(knobPin);
        currentState->knob = knob;
      }
    }
    //screen
    if (lastScreenUpdate + SCREEN_UPDATE_PERIOD <= now) {
      lastScreenUpdate = now;
      uint16_t knob = analogRead(knobPin);
      uint8_t mix = knobToReverbMix(knob);
      //8, 9, 10
      reverbMessage[8] = (mix / 100)? '1' : ' ';
      reverbMessage[9] = (mix / 10) % 10 + '0';
      reverbMessage[10] = mix % 10 + '0';
      sendMessageToScreen(&reverbMessage[0], MESSAGE_SIZE);
    }
  }

  if (lastDelayEnvelopeOn + DELAY_ENVELOPE_ON_PERIOD <= now && effectsFlags & 0x01) {
    lastDelayEnvelopeOn = now;
    delayEnvelope.noteOn();
  }
  if (lastDelayEnvelopeOff + DELAY_ENVELOPE_ON_PERIOD <= now) {
    lastDelayEnvelopeOff = now;
    delayEnvelope.noteOff();
  }
  if (effectsFlags & 0x02) {
    uint8_t wet = knobToReverbMix(stateTable[2].knob);
    reverbMixer.gain(0, (float)wet / 100);
    reverbMixer.gain(1, 1 - ((float)wet / 100));
  } else {
    reverbMixer.gain(0, 0);
    reverbMixer.gain(1, 1);
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



/** Get the next state in the FSM
 * currentState -> pointer to the current state
 * returns a pointer to the next state
 */
struct State* getNextState(struct State* currentState) {
  if (currentState == 0)
    return &stateTable[0];
  return (*currentState).nextState;
}

void setDelayPeriod(uint16_t newDP) {
  delayPeriod = newDP;
  delayEffect.delay(0, delayPeriod);
  delayEffect.delay(1, delayPeriod * 2);
  delayEffect.delay(2, delayPeriod * 3);
}


uint16_t knobToDelayTime(uint16_t knobValue) {
  //convert 521-1023 to 100-500
  return (uint16_t)((knobValue - 521) * 400 / (float)502) + 100;
}

uint8_t knobToReverbMix(uint16_t knobValue) {
  //convert 521-1023 to 0-100
  return (uint16_t)((knobValue - 521) * 100 / (float)502);
}