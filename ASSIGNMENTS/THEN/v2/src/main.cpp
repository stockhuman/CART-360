#include <Audio.h>
#include <SD.h>
#include <SD_t3.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>

#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 7
#define SDCARD_SCK_PIN 14

// Audio IO
AudioOutputI2S i2s_out;
AudioInputAnalogStereo adc_in;
AudioConnection audioL(adc_in, 0, i2s_out, 0);
AudioConnection audioR(adc_in, 1, i2s_out, 1);

// Audio Effects
AudioEffectEnvelope env;
int noteTimeout = 2000;
int envelopeButton = 0;

// SD card playback
AudioPlaySdWav audioSD;
AudioRecordQueue queue;
AudioConnection patchCord3(adc_in, 0, queue, 0);
AudioConnection patchCord4(audioSD, 0, i2s_out, 0);
AudioConnection patchCord5(audioSD, 0, i2s_out, 1);

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

int mode = 0; // 0:
File frec;
elapsedMillis msecs;


void setup()
{
  Serial.begin(115200);  // Debug w/ PC
  Serial1.begin(115200); // Communication with ESP2688

  AudioMemory(60);
  // AudioNoInterrupts();

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

extern void startRecording();
extern void continueRecording();
extern void writeOutHeader();
extern void stopRecording();
extern void startPlaying();
extern void stopPlaying();

void loop() {

  // Send bytes from ESP8266 to computer
  if (Serial1.available() > 0) {
    Serial.write(Serial1.read());
  }

  // Send bytes from computer back to ESP8266
  if (Serial.available() > 0) {
    Serial1.write(Serial.read());
  }

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
