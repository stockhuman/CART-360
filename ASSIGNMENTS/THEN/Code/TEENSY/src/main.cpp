/**
 * Please refer to the visual documentation of this project
 * and the Teensy audio library to better understand this code.
 *
 *
 */

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

int fx_bits = 16;

// Last leg mixer
AudioMixer4 mixer;

AudioConnection AudioIn_L(adc_in, 0, filterL, 0);
AudioConnection AudioIn_R(adc_in, 1, filterR, 0);
AudioConnection MixerMono_L(filterL, 0, mixerLR, 0);
AudioConnection MixerMono_R(filterR, 0, mixerLR, 1);
AudioConnection FXIn_0(mixerLR, fx_bitcrusher);
AudioConnection FXIn_1(mixerLR, fx_delay);
AudioConnection FXIn_2(mixerLR, fx_reverb);
AudioConnection FXOut_0(fx_delay, 0, mixer, 1);
AudioConnection FXOut_1(fx_reverb, 0, mixer, 0);
AudioConnection FXOut_2(fx_bitcrusher, 0, mixer, 2);
AudioConnection AudioOut_L(mixer, 0, i2s_out, 0);
AudioConnection AudioOut_R(mixer, 0, i2s_out, 1);
// debug
// AudioConnection AudioOutDEBUG_L(adc_in, 0, i2s_out, 0);
// AudioConnection AudioOutDEBUG_R(adc_in, 1, i2s_out, 1);

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
	AudioNoInterrupts(); // Disables hardware interrupts, allowing more than one parameter change at once.

  // Audio Effects
  fx_reverb.roomsize(0.3); // Seconds, not millis
  fx_reverb.damping(0.2);

  fx_delay.delay(0, 0); // channel, millis
  fx_delay.delay(0, 0);  // channel, millis

	// for(uint8_t i = 1; i < 6; i++)
	// {
	// 	fx_delay.disable(i); // disables taps
	// }


  // //set xbitDepth to 16 and xsampleRate to 44100 for passthru
  fx_bitcrusher.bits(fx_bits); // passthrough
	fx_bitcrusher.sampleRate(44100);

  filterR.setLowpass(0, 2500);
  filterL.setLowpass(0, 2500);

  mixerLR.gain(0, 0.4);
  mixerLR.gain(1, 0.4);
  mixerLR.gain(2, 0.5);
  mixerLR.gain(3, 0.5);

  // // final mixer
  mixer.gain(0, 0.8);
  mixer.gain(1, 0.8);
  mixer.gain(2, 0.8);

  AudioInterrupts(); // reinstates the interrups, which only allow one CPU blocking instruction per cycle.
  AudioMemory(60); // dedicate 60 blocks of memory to the audio objects

  // SD Card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN)))
  {
    // stop here, but print a message
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
  _loop_interactionWifi(); // just listen for messages from the NodeMCU
 // _loop_interactionSD(); // disabled as per the noise described in sd_card.cpp
}


void _loop_interactionSD() {
  // send data only when you receive data:
  if (Serial.available() > 0)
  {
    // read the incoming byte:
    byte incomingByte = Serial.peek();
    // Respond to button presses
    if (incomingByte == '1')
    {
      Serial.println("Record Button Press");
      delay(200);
      if (mode == 2)
        stopPlaying();
      if (mode == 0)
        startRecording();

			Serial.read();
    }
    if (incomingByte == '2')
    {
      Serial.println("Stop Button Press");
      delay(200);
      if (mode == 1)
        stopRecording();
      if (mode == 2)
        stopPlaying();

      Serial.read();
    }
    if (incomingByte == '3')
    {
      Serial.println("Play Button Press");
      delay(200);
      if (mode == 1)
        stopRecording();
      if (mode == 0)
        startPlaying();

      Serial.read();
    }
  }
  if (mode == 1)
  {
    continueRecording();
  }
}

void _loop_interactionWifi() {
  // Send bytes from computer back to ESP8266
  // and bytes from ESP to computer
  if (Serial1.available() > 0)
  {
    String s = Serial1.readString();
    Serial1.println(s);
    // DEBUG

    // Gain was untested from the demo, and was thus not included
    // NOTE: this is effectively very similar code to what is on the ESP
    if (s.startsWith("m")) {
      String c = s.substring(1, 2);
      int channel = c.toInt();

      String amt = s.substring(3, 4);
      float gain = amt.toFloat();
      Serial.println("gain received -->");
      controlGrain(channel, gain);
    }

		// bitcrusher fx
    if (s.startsWith("b")) {
      String c = s.substring(1, 3);
      int bits = c.toInt();
			Serial.println(bits);
      Serial.println("bitcrush received -->");
      fx_bitcrusher.bits(bits);
    }
  }
}

void controlGrain (int channel, float value) {
  // channel must be between 0 and 3
  mixerLR.gain(channel, value/10);
}
