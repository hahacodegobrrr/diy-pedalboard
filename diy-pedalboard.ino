#include <Audio.h>
#include <Wire.h>
#include <SPI.h>

#include "TunerUtils.h"


const int MESSAGE_SIZE = 32; //in bytes
const int SCREEN_UPDATE_PERIOD = 1000; //in ms
const int FFT_BIN_SIZE = 43; //hz

//switch depending on which one we're using
const int input = AUDIO_INPUT_LINEIN;
// const int input = AUDIO_INPUT_MIC;

unsigned int lastScreenUpdate;

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

char message[];

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
  unsigned int now = millis();

  //screen
  if (lastScreenUpdate + SCREEN_UPDATE_PERIOD <= now) {
    if (currentState.id == 0 && fft.available()) { //tuner mode
      int loudestBin = 0;
      float loudest = 0;
      int i;
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
void sendMessageToScreen(char* p_text, int length) {
  Serial1.write(0); //signal beginning of message
  Serial1.write(p_text, length);
}