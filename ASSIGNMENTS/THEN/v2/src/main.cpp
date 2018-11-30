#include <Audio.h>
#include <SD.h>
#include <SD_t3.h>
#include <SPI.h>
#include <Wire.h>

#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 7
#define SDCARD_SCK_PIN 14

// Audio IO
AudioInputAnalogStereo adc_in;
AudioOutputI2S i2s_out;

// Audio Filters
AudioFilterBiquad filterL;
AudioFilterBiquad filterR;
AudioMixer4 mixerLR; // to pseudo-mono

// Audio Effects
AudioEffectDelay fx_delay;
AudioEffectFreeverb fx_reverb;
AudioEffectBitcrusher fx_bitcrusher;

// Last leg mixer
AudioMixer4 mixer;

// AudioConnection AudioIn_L(adc_in, 0, filterL, 0);
// AudioConnection AudioIn_R(adc_in, 1, filterR, 0);
// AudioConnection MixerMono_L(filterL, 0, mixerLR, 0);
// AudioConnection MixerMono_R(filterR, 0, mixerLR, 1);
// AudioConnection FXIn_0(mixerLR, fx_bitcrusher);
// AudioConnection FXIn_1(mixerLR, fx_delay);
// AudioConnection FXIn_2(mixerLR, fx_reverb);
// AudioConnection FXOut_0(fx_delay, 0, mixer, 1);
// AudioConnection FXOut_1(fx_reverb, 0, mixer, 0);
// AudioConnection FXOut_2(fx_bitcrusher, 0, mixer, 2);
// AudioConnection AudioOut_L(mixer, 0, i2s_out, 0);
// AudioConnection AudioOut_R(mixer, 0, i2s_out, 1);
// debug
AudioConnection AudioOutDEBUG_L(adc_in, 0, i2s_out, 0);
AudioConnection AudioOutDEBUG_R(adc_in, 1, i2s_out, 1);

// SD card playback
AudioPlaySdWav audioSD;
AudioRecordQueue queue;
AudioConnection AudioSDIn(mixerLR, queue);
AudioConnection AudioSDOut_L(audioSD, 0, mixerLR, 2);
AudioConnection AudioSDOut_R(audioSD, 1, mixerLR, 3);

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
int mode = 0;
File frec;
elapsedMillis msecs;


void setup()
{
  // Communication between MCUs
  Serial.begin(115200);  // Debug w/ PC
  Serial1.begin(115200); // Communication with ESP2688

	// await signal from ESP
	// while(!Serial1.available()){
	// 	byte startCommand = 'R'; // for ready
	// 	byte incomingByte = Serial1.read();
	// 	if (startCommand == incomingByte) {
	// 		Serial.println("ESP Ready");
	// 		break;
	// 	}
	// }
	// AudioNoInterrupts();

  // Audio Effects
  // fx_reverb.roomsize(0.5); // Seconds, not millis
  // fx_reverb.damping(0.3);

  // fx_delay.delay(0, 0); // channel, millis
  // fx_delay.delay(0, 0);  // channel, millis

	// // for(uint8_t i = 1; i < 6; i++)
	// // {
	// // 	fx_delay.disable(i); // disables taps
	// // }


  // //set xbitDepth to 16 and xsampleRate to 44100 for passthru
  // fx_bitcrusher.bits(16); // passthrough
	// fx_bitcrusher.sampleRate(44100);

  // filterR.setLowpass(0, 500);
  // filterL.setLowpass(0, 500);

  // mixerLR.gain(0, 1);
  // mixerLR.gain(1, 1);
  // mixerLR.gain(2, 1);
  // mixerLR.gain(3, 1);

  // // final mixer
  // mixer.gain(0, 1);
  // mixer.gain(1, 1);
  // mixer.gain(2, 1);

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
  mixerLR.gain(channel, value/10);
}
