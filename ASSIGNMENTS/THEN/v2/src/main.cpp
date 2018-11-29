#include <Audio.h>

#include <Arduino.h>
#include <SD.h>
#include <SD_t3.h>
#include <SPI.h>
#include <Wire.h>
#include <ssd1351.h>
#define SLOW_SPI
#define SDCARD_CS_PIN BUILTIN_SDCARD
#define SDCARD_MOSI_PIN 7
#define SDCARD_SCK_PIN 14

typedef ssd1351::LowColor Color;

// Choose display buffering - NoBuffer or SingleBuffer currently supported
// auto display = ssd1351::SSD1351<Color, ssd1351::NoBuffer, 128, 96>();
auto display = ssd1351::SSD1351<Color, ssd1351::SingleBuffer, 128, 96>();

bool up = false;
int pos = 127;
const int particles = 256;
int offsets[particles];
int x_pos[particles];
int y_pos[particles];
Color particle_colors[particles];

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

// Additional recording variables
int mode = 0; // 0:
File frec;
elapsedMillis msecs;


void setup()
{
  Serial.begin(115200);  // Debug w/ PC
  Serial1.begin(115200); // Communication with ESP2688
  display.begin();
  for (int i = 0; i < particles; i++) {
    x_pos[i] = random(0, 128);
    y_pos[i] = random(0, 96);
    particle_colors[i] = ssd1351::RGB(0, i + 10, i / 2 + 10);
  }
  AudioMemory(60);

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

  unsigned long before = millis();
  display.fillScreen(ssd1351::RGB());
  Color circleColor = ssd1351::RGB(0, 128, 255);

  for (int i = 0; i < particles; i++) {
    offsets[i] += random(-2, 3);
    display.drawLine(x_pos[i] + offsets[i], y_pos[i] + offsets[i], pos,
                     80 + sin(pos / 4.0) * 20, particle_colors[i]);
    display.drawCircle(x_pos[i] + offsets[i], y_pos[i] + offsets[i], 1,
                       circleColor);
  }
  display.updateScreen();
  Serial.println(millis() - before);

  if (up) {
    pos++;
    if (pos >= 127) {
      up = false;
    }
  } else {
    pos--;
    if (pos < 0) {
      up = true;
    }
  }

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
