#include <Arduino.h>
#line 1 "c:\\Users\\Mike\\Desktop\\THEN\\THEN.ino"
#line 1 "c:\\Users\\Mike\\Desktop\\THEN\\THEN.ino"
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Define the fancy console
#include "debug.h"
// Console consol;

// Initialize 64 x 32 display on pins D3 (SDA) and D5 (SCK)
#include "SSD1306.h"
SSD1306 display(0x3c, D3, D5);

#line 12 "c:\\Users\\Mike\\Desktop\\THEN\\THEN.ino"
void setup();
#line 19 "c:\\Users\\Mike\\Desktop\\THEN\\THEN.ino"
void loop();
#line 12 "c:\\Users\\Mike\\Desktop\\THEN\\THEN.ino"
void setup() {
  Serial.begin(19200);
  display.init();
  // display.invertDisplay();
  display.flipScreenVertically();
}

void loop() {
  display.display();
  display.drawString(0, 32, "Temp: " + String(44) + "F");
  display.drawProgressBar(4, 8, 10, 8, 2);
  delay(1000);
}

