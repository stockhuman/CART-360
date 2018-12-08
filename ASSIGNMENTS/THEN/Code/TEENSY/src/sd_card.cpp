/**
 * Audio recording to built-in Teensy SD card via Paul S. of PJRC fame.
 * The following functions were not demonstrated in the demo due to high noise during playback
 * and audio artefacting.
 * I have added the majority of the comments
 */

// dependency graph reolves this away, not to worry about redundancy.
#include <Audio.h>
#include <SD.h>
#include <SD_t3.h>
#include <SPI.h>
#include <SerialFlash.h>
#include <Wire.h>

// SD Card variables (declared in main.cpp)
extern unsigned long ChunkSize;
extern unsigned long Subchunk1Size;
extern unsigned int AudioFormat;
extern unsigned int numChannels;
extern unsigned long sampleRate;
extern unsigned int bitsPerSample;
// samplerate x channels x (bitspersample / 8)
extern unsigned long byteRate;
extern unsigned int blockAlign;
extern unsigned long Subchunk2Size;
extern unsigned long recByteSaved;
extern unsigned long NumSamples;
extern byte byte1, byte2, byte3, byte4;

extern int mode; // 0: stopped, 1: recording, 2: playing
extern File frec;
extern elapsedMillis msecs;

extern AudioPlaySdWav audioSD;
extern AudioRecordQueue queue;

void startRecording()
{
  Serial.println("startRecording");
  // all files must be all caps, and under 8 characters long before extension (DOS, basically)
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
    byte buffer[512]; // initialize a byte array buffer
    memcpy(buffer, queue.readBuffer(), 256); // copy the que to the buffer
    queue.freeBuffer(); // flush queue halfway (perf optimisation)
    memcpy(buffer + 256, queue.readBuffer(), 256);
    queue.freeBuffer();
    // write all 512 bytes to the SD card
    frec.write(buffer, 512);
    recByteSaved += 512; // used in SD card fsize tally
  }
}

// A series of metadata bitmasks to create the filesize, format and name of the file in header
void writeOutHeader()
{ // update WAV header with final filesize/datasize

  NumSamples = (recByteSaved*8)/bitsPerSample/numChannels;
  // number of samples x number of channels x number of bytes per sample
  Subchunk2Size = NumSamples*numChannels*bitsPerSample/8;
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

// Writes out the last of the queue and creates the file
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

// reads from SD card. NOTE: terrible audio glitches :/
void startPlaying()
{
  Serial.println("startPlaying");
  audioSD.play("RECORD.WAV");
  mode = 2;
}

// stops the file, if playing
void stopPlaying()
{
  Serial.println("stopPlaying");
  if (mode == 2)
    audioSD.stop();
  mode = 0;
}
