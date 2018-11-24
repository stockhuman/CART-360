#include <SD.h>
#include <SD_t3.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Audio.h>
#include <Wire.h>

#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 7
#define SDCARD_SCK_PIN 14

// Audio IO
AudioOutputI2S i2s_out;
AudioInputI2S  i2s_in;

// Audio Effects
AudioEffectEnvelope env;
int noteTimeout = 2000;
int envelopeButton = 0;


// from module in to module out
// AudioConnection patchCord1(i2s_in, 0, i2s_out, 0);
// AudioConnection patchCord2(i2s_in, 1, i2s_out, 1);
// AudioConnection patchCord1(i2s_in, 0, env, 0);
AudioConnection patchCord2(i2s_in, 0, i2s_out, 0);
AudioConnection patchCord6(i2s_in, 1, i2s_out, 1);

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

int mode = 0; // 0: stopped, 1: recording, 2: playing
File frec;
elapsedMillis msecs;

// SD card playback
AudioPlaySdWav audioSD;
AudioRecordQueue queue;
AudioConnection patchCord3(i2s_in, 0, queue, 0);
AudioConnection patchCord4(audioSD, 0, i2s_out, 0);
AudioConnection patchCord5(audioSD, 0, i2s_out, 1);


void setup()
{
  Serial.begin(9600);
  AudioMemory(60);
  pinMode(0, INPUT);
  pinMode(1, INPUT);
  pinMode(2, INPUT);
  AudioNoInterrupts();
  // sine.frequency(440);
  // sine.amplitude(0.5);
  // AudioInterrupts();

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

void startRecording()
{
  Serial.println("startRecording");
  if (SD.exists("RECORD.WAV"))
  {
    SD.remove("RECORD.WAV");
  }
  frec = SD.open("RECORD.WAV", FILE_WRITE);
  if (frec)
  {
    queue.begin();
    mode = 1;
    recByteSaved = 0L;
  }
}

void continueRecording()
{

  if (queue.available() >= 2)
  {
    byte buffer[512];
    memcpy(buffer, queue.readBuffer(), 256);
    queue.freeBuffer();
    memcpy(buffer + 256, queue.readBuffer(), 256);
    queue.freeBuffer();
    // write all 512 bytes to the SD card
    frec.write(buffer, 512);
    recByteSaved += 512;
    //    elapsedMicros usec = 0;
    //    Serial.print("SD write, us=");
    //    Serial.println(usec);
  }
}

void writeOutHeader()
{ // update WAV header with final filesize/datasize

  //  NumSamples = (recByteSaved*8)/bitsPerSample/numChannels;
  //  Subchunk2Size = NumSamples*numChannels*bitsPerSample/8; // number of samples x number of channels x number of bytes per sample
  Subchunk2Size = recByteSaved;
  ChunkSize = Subchunk2Size + 36;
  frec.seek(0);
  frec.write("RIFF");
  byte1 = ChunkSize & 0xff;
  byte2 = (ChunkSize >> 8) & 0xff;
  byte3 = (ChunkSize >> 16) & 0xff;
  byte4 = (ChunkSize >> 24) & 0xff;
  frec.write(byte1);
  frec.write(byte2);
  frec.write(byte3);
  frec.write(byte4);
  frec.write("WAVE");
  frec.write("fmt ");
  byte1 = Subchunk1Size & 0xff;
  byte2 = (Subchunk1Size >> 8) & 0xff;
  byte3 = (Subchunk1Size >> 16) & 0xff;
  byte4 = (Subchunk1Size >> 24) & 0xff;
  frec.write(byte1);
  frec.write(byte2);
  frec.write(byte3);
  frec.write(byte4);
  byte1 = AudioFormat & 0xff;
  byte2 = (AudioFormat >> 8) & 0xff;
  frec.write(byte1);
  frec.write(byte2);
  byte1 = numChannels & 0xff;
  byte2 = (numChannels >> 8) & 0xff;
  frec.write(byte1);
  frec.write(byte2);
  byte1 = sampleRate & 0xff;
  byte2 = (sampleRate >> 8) & 0xff;
  byte3 = (sampleRate >> 16) & 0xff;
  byte4 = (sampleRate >> 24) & 0xff;
  frec.write(byte1);
  frec.write(byte2);
  frec.write(byte3);
  frec.write(byte4);
  byte1 = byteRate & 0xff;
  byte2 = (byteRate >> 8) & 0xff;
  byte3 = (byteRate >> 16) & 0xff;
  byte4 = (byteRate >> 24) & 0xff;
  frec.write(byte1);
  frec.write(byte2);
  frec.write(byte3);
  frec.write(byte4);
  byte1 = blockAlign & 0xff;
  byte2 = (blockAlign >> 8) & 0xff;
  frec.write(byte1);
  frec.write(byte2);
  byte1 = bitsPerSample & 0xff;
  byte2 = (bitsPerSample >> 8) & 0xff;
  frec.write(byte1);
  frec.write(byte2);
  frec.write("data");
  byte1 = Subchunk2Size & 0xff;
  byte2 = (Subchunk2Size >> 8) & 0xff;
  byte3 = (Subchunk2Size >> 16) & 0xff;
  byte4 = (Subchunk2Size >> 24) & 0xff;
  frec.write(byte1);
  frec.write(byte2);
  frec.write(byte3);
  frec.write(byte4);
  frec.close();
  Serial.println("header written");
  Serial.print("Subchunk2: ");
  Serial.println(Subchunk2Size);
}

void stopRecording()
{
  Serial.println("stopRecording");
  queue.end();
  if (mode == 1)
  {
    while (queue.available() > 0)
    {
      frec.write((byte *)queue.readBuffer(), 256);
      queue.freeBuffer();
      recByteSaved += 256;
    }
    writeOutHeader();
    frec.close();
  }
  mode = 0;
}

void startPlaying()
{
  Serial.println("startPlaying");
  audioSD.play("RECORD.WAV");
  mode = 2;
}

void stopPlaying()
{
  Serial.println("stopPlaying");
  if (mode == 2)
    audioSD.stop();
  mode = 0;
}

void loop()
{

  // if (digitalRead(0) == HIGH) {
  //   env.noteOn();
  //   Serial.println("Note on");
  // }

  // int pot = analogRead(A0);
  // env.hold(pot);

  // send data only when you receive data:
  //if (Serial.available() > 0)
  // if (digitalRead(0) == HIGH)
  // {
    // read the incoming byte:
    // byte incomingByte = Serial.read();
    // Respond to button presses
    // if (incomingByte == '1')
    if (digitalRead(0) == HIGH)
    {
      Serial.println("Record Button Press");
      delay(200);
      if (mode == 2)
        stopPlaying();
      if (mode == 0)
        startRecording();
    }
    // if (incomingByte == '2')
    if (digitalRead(1) == HIGH)
    {
      Serial.println("Stop Button Press");
      delay(200);
      if (mode == 1)
        stopRecording();
      if (mode == 2)
        stopPlaying();
    }
    // if (incomingByte == '3')
    if (digitalRead(2) == HIGH)
    {
      Serial.println("Play Button Press");
      delay(200);
      if (mode == 1)
        stopRecording();
      if (mode == 0)
        startPlaying();
    }
  // }
  if (mode == 1)
  {
    continueRecording();
  }
}
