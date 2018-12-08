#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Define the fancy console
#include "debug.h"
// Console consol;

static const unsigned char PROGMEM logo_bmp[] = {
  B11000000, B00000000,
  B11000000, B00000001,
  B11000000, B00000001,
  B11100000, B00000011,
  B11100000, B11110011,
  B11111000, B11111110,
  B11111111, B01111110,
  B10011111, B00110011,
  B11111100, B00011111,
  B01110000, B00001101,
  B10100000, B00011011,
  B11100000, B00111111,
  B11110000, B00111111,
  B11110000, B01111100,
  B01110000, B01110000,
  B00110000, B00000000
};

boolean postSplash = false;

// Initialize 64 x 32 display on pins D3 (SDA) and D5 (SCK)
#include "SSD1306.h"
SSD1306 display(0x3c, D3, D5);

void setup() {
  Serial.begin(19200);
  pinMode(A0, INPUT); // declare the only analog in as input
  display.init();
  // display.invertDisplay();
  display.flipScreenVertically();
}

void loop() {
  if (!postSplash) {
    splashScreen();
  }

  display.clear();

  display.drawString(0, 32, "Booting...");
  Serial.println(analogRead(A0));
  display.drawProgressBar(4, 8, SCREEN_WIDTH - 8, 8, map(analogRead(A0), 2, 1023, 0, 100));
  display.display();
  delay(10);
}

void splashScreen() {
  display.drawFastImage(SCREEN_WIDTH / 2 - 8, SCREEN_HEIGHT / 2 - 8, 16, 16, logo_bmp);
  display.display();
  postSplash = true;
  delay(3000);
}
