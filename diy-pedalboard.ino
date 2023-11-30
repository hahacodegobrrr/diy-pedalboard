#include <Audio.h>
#include <Wire.h>
#include <SPI.h>

#include <stdint.h>

// #include "TunerUtils.h"
// #include "FSM.h"
struct State {
  uint8_t id;
  uint16_t knob1;
  uint16_t knob2;
  uint16_t knob3;
  struct State* nextState;
};

const uint8_t MESSAGE_SIZE = 32; //in bytes
const uint16_t SCREEN_UPDATE_PERIOD = 1000; //in ms
const float FFT_BIN_SIZE = 43; //hz

//switch depending on which one we're using
const uint8_t input = AUDIO_INPUT_LINEIN;
// const uint8_t input = AUDIO_INPUT_MIC;

uint64_t lastScreenUpdate;

//audio shit
AudioInputI2S audioInput;   //audio shield line/mic in
AudioOutputI2S audioOutput;  //audio shield line out
AudioControlSGTL5000 audioShield;

AudioAnalyzeFFT1024 fft;

//a module cannot have multiple inputs -> send to a mixer
//a module can have multiple outputs
AudioConnection patchCord1(audioInput, 0, fft, 0);
AudioConnection patchCord2(fft, 0, audioOutput, 0);

struct State* currentState;

uint8_t screenMessage[MESSAGE_SIZE];

void setup(){
  AudioMemory(120); //change later (probably needs to increase)
  audioShield.enable();
  audioShield.inputSelect(input);
  audioShield.volume(0.5);

  fft.windowFunction(AudioWindowHanning1024);

  currentState = getNextState(0);

  Serial1.begin(9600);
  lastScreenUpdate = millis();
}

void loop(){
  uint64_t now = millis();
  //screen
  if (lastScreenUpdate + SCREEN_UPDATE_PERIOD <= now) {
    if (currentState->id == 0 && fft.available()) { //tuner mode
      uint16_t loudestBin = 0;
      float loudest = 0;
      uint16_t i;
      for (i = 0; i < 40; i++) {
        float bin = fft.read(i);
        Serial.print(bin);
        Serial.print("-");

        if (bin > loudest) {
          loudest = bin;
          loudestBin = i;
        }
      }
      Serial.println();
      //could try linear interpolation here or smth
      uint8_t* msg = generateTunerDisplayMessage(loudestBin * FFT_BIN_SIZE);
      sendMessageToScreen(msg, MESSAGE_SIZE);
    }
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


//from tunerutils

#include <math.h>

#define MESSAGE_SIZE 32

struct TunerNote {
  uint8_t note;
  float error;
};

const float ln2 = 0.69314718f;
const float FREQ_A4 = 440;

uint8_t noteTable[] = {'A','a','B','C','c','D','d','E','F','f','G','g'};

struct TunerNote tunerNote;
uint8_t tunerMessage[MESSAGE_SIZE] = {'T','U','N','E','R',' ',' ',' ','N','O','T','E',':',' '};

void getNote(uint16_t frequency) {
  float freqTo12Tone = (float)(fmod(log(frequency / FREQ_A4) / ln2, 1)) * 12;
  
  float error = fmod(freqTo12Tone, 1);
  uint8_t note;
  if (error >= 0.5) {
    error -= 1; //wrap around to next note's error
    note = ((uint8_t)freqTo12Tone + 1) % 12;
  } else {
    note = (uint8_t)freqTo12Tone;
  }

  tunerNote.note = note;
  tunerNote.error = error;
}

uint8_t* generateTunerDisplayMessage(uint16_t frequency) {
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
    tunerMessage[i + MESSAGE_SIZE / 2] = (tunerNote.error > 0)? (transI < tunerNote.error && transI > 0)? '-' : ' ' : (transI > tunerNote.error && transI < 0)? '-' : ' ';
  }

  return &tunerMessage[0];
}

//from fsm
//tuner, chorus, delay, freeverb


struct State stateTable[] = {
  {.id = 0, .knob1 = 0, .knob2 = 0, .knob3 = 0, .nextState = &stateTable[1]},
  {.id = 1, .knob1 = 0, .knob2 = 0, .knob3 = 0, .nextState = &stateTable[2]},
  {.id = 2, .knob1 = 0, .knob2 = 0, .knob3 = 0, .nextState = &stateTable[3]},
  {.id = 3, .knob1 = 0, .knob2 = 0, .knob3 = 0, .nextState = &stateTable[0]}
};

struct State* getNextState(struct State* currentState) {
  if (currentState == 0)
    return &stateTable[0];
  return (*currentState).nextState;
}