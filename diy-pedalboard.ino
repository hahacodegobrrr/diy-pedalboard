#include <Audio.h>
#include <Wire.h>
#include <SPI.h>

#include <stdint.h>

#include "TunerUtils.h"
#include "FSM.h"


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
        if (bin > loudest) {
          loudest = bin;
          loudestBin = i;
        }
      }
      //could try linear interpolation here or smth
      sendMessageToScreen(generateTunerDisplayMessage(loudestBin * FFT_BIN_SIZE), MESSAGE_SIZE);
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