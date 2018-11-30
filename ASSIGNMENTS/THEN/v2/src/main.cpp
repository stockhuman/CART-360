#include <Audio.h>
#include <SD.h>
#include <SD_t3.h>
#include <SPI.h>
#include <Wire.h>

#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 7
#define SDCARD_SCK_PIN 14

// Audio IO
// AudioOutputI2S i2s_out;
// AudioInputAnalogStereo adc_in;
// AudioConnection audioL(adc_in, 0, i2s_out, 0);
// AudioConnection audioR(adc_in, 1, i2s_out, 1);

// GUItool: begin automatically generated code
AudioInputAnalogStereo adc_in;  //xy=62,267.3666687011719
AudioFilterBiquad filterR;      //xy=254.36666870117188,371.3666687011719
AudioFilterBiquad filterL;      //xy=269.3666687011719,140.36666870117188
AudioEffectFreeverb fx_reverbL; //xy=464.3666687011719,41.366668701171875
AudioEffectDelay fx_delayL;     //xy=474.3666687011719,134.36666870117188
AudioEffectDelay fx_delayR;     //xy=479.3666687011719,486.3666687011719
AudioEffectFreeverb fx_reverbR; //xy=488.3666687011719,366.3666687011719
AudioMixer4 mixerR;             //xy=693.36669921875,342.3666687011719
AudioMixer4 mixerL;             //xy=695.36669921875,205.36666870117188
AudioOutputI2S i2s_out;         //xy=920.4334716796875,265.5697326660156
AudioConnection patchCord1(adc_in, 0, mixerL, 0);
AudioConnection patchCord2(adc_in, 0, filterL, 0);
AudioConnection patchCord3(adc_in, 1, mixerR, 0);
AudioConnection patchCord4(adc_in, 1, filterR, 0);
AudioConnection patchCord5(filterR, fx_delayR);
AudioConnection patchCord6(filterR, fx_reverbR);
AudioConnection patchCord7(filterL, fx_delayL);
AudioConnection patchCord8(filterL, fx_reverbL);
AudioConnection patchCord9(fx_reverbL, 0, mixerL, 1);
AudioConnection patchCord10(fx_delayL, 0, mixerL, 2);
AudioConnection patchCord11(fx_delayR, 0, mixerR, 2);
AudioConnection patchCord12(fx_reverbR, 0, mixerR, 1);
AudioConnection patchCord13(mixerR, 0, i2s_out, 1);
AudioConnection patchCord14(mixerL, 0, i2s_out, 0);
// GUItool: end automatically generated code

// Audio Effects

// SD card playback
AudioPlaySdWav audioSD;
AudioRecordQueue queue;
AudioConnection patch_queue(adc_in, 0, queue, 0);
AudioConnection patch_SDOutL(audioSD, 0, i2s_out, 0);
AudioConnection patch_SDOutR(audioSD, 0, i2s_out, 1);

// SD Card variables
unsigned long ChunkSize = 0L;
unsigned long Subchunk1Size = 16;
unsigned int AudioFormat = 1;
unsigned int numChannels = 1;
unsigned long sampleRate = 44100;
unsigned int bitsPerSample = 16;
// samplerate x channels x (bitspersample / 8)
unsigned long byteRate = sampleRate * numChannels * (bitsPerSample / 8);
unsigned int blockAlign = numChannels * bitsPerSample / 8;
unsigned long Subchunk2Size = 0L;
unsigned long recByteSaved = 0L;
unsigned long NumSamples = 0L;
byte byte1, byte2, byte3, byte4;

// Additional recording variables
int mode = 0; // 0:
File frec;
elapsedMillis msecs;


void setup()
{
  // Communication between MCUs
  Serial.begin(115200);  // Debug w/ PC
  Serial1.begin(115200); // Communication with ESP2688

  // Audio Effects
  fx_reverbL.roomsize(0.5); // Seconds, not millis
  fx_reverbL.damping(0.3);
  fx_reverbR.roomsize(0);
  fx_reverbR.damping(1);

  fx_delayL.delay(0, 10); // channel, millis
  fx_delayR.delay(0, 0);    // channel, millis
  fx_delayL.disable(2); // disables taps

  //set xbitDepth to 16 and xsampleRate to 44100 for passthru
  // bitcrusher.bits(16); // passthrough
  // ampL.gain(0.2);
  // ampR.gain(0.2);
  filterR.setLowpass(0, 700);
  filterL.setLowpass(0, 900);

  mixerL.gain(0, 0);
  mixerL.gain(1, 1);
  mixerL.gain(2, 0);

  mixerR.gain(0, 0);
  mixerR.gain(1, 0.5);
  mixerR.gain(2, 0.5);

  // AudioInterrupts();
  AudioMemory(60);

  // SD Card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN)))
  {
    // stop here, but print a message repetitively
    while (!(SD.begin(SDCARD_CS_PIN)))
    {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
}

// SD Card fns, see sd_card.cpp
extern void startRecording();
extern void continueRecording();
extern void writeOutHeader();
extern void stopRecording();
extern void startPlaying();
extern void stopPlaying();

// WiFi control functions


// foward declarations for the loop
void _loop_interactionSD();
void _loop_interactionWifi();
void controlGrain(int, float);

void loop() {
  _loop_interactionWifi();
  _loop_interactionSD();
}


void _loop_interactionSD() {
  // send data only when you receive data:
  if (Serial.available() > 0)
  {
    // read the incoming byte:
    byte incomingByte = Serial.read();
    // Respond to button presses
    if (incomingByte == '1')
    {
      Serial.println("Record Button Press");
      delay(200);
      if (mode == 2)
        stopPlaying();
      if (mode == 0)
        startRecording();
    }
    if (incomingByte == '2')
    {
      Serial.println("Stop Button Press");
      delay(200);
      if (mode == 1)
        stopRecording();
      if (mode == 2)
        stopPlaying();
    }
    if (incomingByte == '3')
    {
      Serial.println("Play Button Press");
      delay(200);
      if (mode == 1)
        stopRecording();
      if (mode == 0)
        startPlaying();
    }
  }
  if (mode == 1)
  {
    continueRecording();
  }
}

void _loop_interactionWifi() {
  // Send bytes from ESP8266 to computer
  if (Serial1.available() > 0)
  {
    Serial.write(Serial1.read());
  }

  // Send bytes from computer back to ESP8266
  if (Serial.available() > 0)
  {
    Serial1.write(Serial.read());
    // DEBUG
    String s = Serial.readString();
    if (s.startsWith("m")) {
      String c = s.substring(1, 2);
      int channel = c.toInt();

      String amt = s.substring(3, 4);
      float gain = amt.toFloat();
      Serial.println("gain received -->");
      controlGrain(channel, gain);
    }

  }
}

void controlGrain (int channel, float value) {
  // channel must be between 0 and 3
  mixerL.gain(channel, value);
}