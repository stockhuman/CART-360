// Assignment THIS
// Michael Hemingway
// 2018.10.19 12:30PM

#define tonePin 11 // Set this to be the pin that your buzzer resides in.
const int thermoPin = 0; // Analog pin zero

void setup() {
  Serial.begin(9600);
}


void loop() {
  int thermoRead = analogRead(thermoPin);
   
  // Play midi
  midi();
}
