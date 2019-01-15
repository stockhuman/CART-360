#define MUX_A 2
#define MUX_B 0
#define MUX_C 4

const int RGB_R = 15;
const int RGB_G = 13;
const int RGB_B = 12;

#define ANALOG_INPUT A0

void setup() {
  Serial.begin(115200);
  // avoids the next line concatenating with the large string of garbled text at boot
  Serial.println("");
  Serial.println("ANALOG READ TEST");

  pinMode(MUX_A, OUTPUT);
  pinMode(MUX_B, OUTPUT);     
  pinMode(MUX_C, OUTPUT);

  pinMode(RGB_R, OUTPUT);
  pinMode(RGB_G, OUTPUT);
  pinMode(RGB_B, OUTPUT);
}

int readMux(const byte select) {
  // select desired MUX channel
  digitalWrite(MUX_A, (select & 1) ? HIGH : LOW); // low-order bit
  digitalWrite(MUX_B, (select & 2) ? HIGH : LOW);
  digitalWrite(MUX_C, (select & 4) ? HIGH : LOW); // high-order bit

  // now read the sensor
  return analogRead(ANALOG_INPUT);
}

void loop() {
  Serial.print("000: ");

  digitalWrite(MUX_A, LOW); // low-order bit
  digitalWrite(MUX_B, LOW);
  digitalWrite(MUX_C, LOW); // high-order bit
  analogWrite(RGB_R, map(analogRead(ANALOG_INPUT), 0, 1024, 0, 255));
  Serial.println(analogRead(ANALOG_INPUT));

  Serial.print("001: ");
  digitalWrite(MUX_C, HIGH); // low-order bit
  analogWrite(RGB_G, map(analogRead(ANALOG_INPUT), 0, 1024, 0, 255));
  Serial.println(analogRead(ANALOG_INPUT));

  Serial.print("010: ");
  digitalWrite(MUX_C, LOW); // low-order bit
  digitalWrite(MUX_B, HIGH);
  analogWrite(RGB_B, map(analogRead(ANALOG_INPUT), 0, 1024, 0, 255));
  Serial.println(analogRead(ANALOG_INPUT));
}
