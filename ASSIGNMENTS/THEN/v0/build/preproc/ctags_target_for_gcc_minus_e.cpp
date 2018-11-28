# 1 "c:\\Users\\Mike\\Desktop\\THEN\\THEN.ino"
# 1 "c:\\Users\\Mike\\Desktop\\THEN\\THEN.ino"



// Define the fancy console
# 6 "c:\\Users\\Mike\\Desktop\\THEN\\THEN.ino" 2
// Console consol;

// Initialize 64 x 32 display on pins D3 (SDA) and D5 (SCK)
# 10 "c:\\Users\\Mike\\Desktop\\THEN\\THEN.ino" 2
SSD1306 display(0x3c, D3, D5);

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
