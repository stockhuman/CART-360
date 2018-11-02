#define BUTTON_MODE_PIN 2  // Button to change the mode

// constants for RGB LED
#define LED_PIN_R 5 // R PIN
#define LED_PIN_G 9 // G PIN
#define LED_PIN_B 6 // B PIN

// constant for note in (button-resistor ladder on breadboard)
#define NOTE_IN_PIN A0

//constant for max number of notes in array
#define MAX_NOTES 16

// a constant for duration
const int duration = 200;

// constant for pin to output for buzzer
#define BUZZER_PIN 3 // PWM

int countNotes = 0; // counter for how many notes we have
int mode = 0; // start at off
int notes [MAX_NOTES]; // array to hold the notes played (for record/play mode)


// for ease of debugging: these aare utility functions for nice logging
void l(String = "", String = "", String = "", String = "");
void l(String s, String s2) { Serial.print(s); Serial.println(s2); }
void l(String s, int i) { Serial.print(s); Serial.println(i); }
void l(int i) { Serial.println(i); }
void l(String s, String s2, String s3, String s4) {
  Serial.print(s + ' ');
  Serial.print(s2 + ", ");
  Serial.print(s3 + ", ");
  Serial.println(s4 + '.');
}
void m () { // logs the current mode
  switch (mode)
  {
    case 0: Serial.println("Reset--"); break;
    case 1: Serial.println("Live()--"); break;
    case 2: Serial.println("Record()--"); break;
    case 3: Serial.println("Play()--"); break;
    case 4: Serial.println("Looper()--");  break;
    default:
      break;
  }
}
void int_array_to_string(int int_array[], int size_of_array)
{
  String returnstring = "";
  for (int temp = 0; temp < size_of_array; temp++)
    returnstring += String(int_array[temp]) + " ";
  Serial.println(returnstring);
}
//  end debug methods

void setup()
{
  pinMode(BUTTON_MODE_PIN, INPUT);
	Serial.begin(9600);
}


void loop()
{
  chooseMode();
  setRGB();
  selectMode();
}

void chooseMode() {
  int buttonState = digitalRead(BUTTON_MODE_PIN);
  if (buttonState == 1) { // test if the mode switch button is pressed
    mode = (mode + 1) % 5; // increment the mode by one, and modulo-wrap it around to 0
    l("mode ", mode);
    m();
    delay(250); // debouncing without variables!
  }
}

void setRGB()
{
  // resets all LED component colors
  analogWrite(LED_PIN_B, 0);
  analogWrite(LED_PIN_G, 0);
  analogWrite(LED_PIN_R, 0);

  switch (mode)
  {
    case 1: analogWrite(LED_PIN_B, 255); break; // set blue on live()
    case 2: analogWrite(LED_PIN_R, 255); break; // set red on record()
    case 3: analogWrite(LED_PIN_G, 255); break; // set green on play()

    case 4: // achieves purple with a mix of red and blue on looper()
      analogWrite(LED_PIN_R, 127);
      analogWrite(LED_PIN_B, 212);
      break;

    default: // keeps the LEDs off during reset()
      break;
  }
}

void selectMode()
{
  // I have optimised this to also use a switch statement,
  // as each case is as likely as the next.
  switch (mode)
  {
    case 0:
      reset();
      break;
    case 1:
      live();
      break;
    case 2:
      record();
      break;
    case 3:
      play();
      break;
    case 4:
      looper();
      break;
  }
}


void reset()
{
  // the performance implications of zeroing out memory on every iteration of
  // loop() seems excessive.
  memset(notes, 0, sizeof(notes));
  delay(30); // tiny pause, for the health of the chip. :)
}


void live()
{
  playnote();
}


void record()
{
  int sensedInput = readnote(); // capture the current note, if any

  if (sensedInput > 10) // simple high-pass
  {
    l("note read: ", sensedInput);

    // get the next place in the array to place a note,
    // another artefact of the no-extra-vars rule.

    // I initially used an iterator variable, but I saw that simply integrating
    // my iterator with this nextAvailableCell var was more concise and expressive
    for (int nextAvailableCell = 0; nextAvailableCell < MAX_NOTES; nextAvailableCell++)
    {
      // bail early if we're at the limit of the tune
      if ( nextAvailableCell == ( MAX_NOTES - 1 ))
      {
        l("MAX NOTES REACHED");
        // because this is cheaper than implementing a linked list every loop
        memset(notes, 0, sizeof(notes));
        notes[0] = sensedInput; // write to the beggining of the array
        l("Recorded: ", notes[0]);
        return;
      }

      // test if current note is uninitialised
      if (notes[nextAvailableCell] == 0) {
        l("Index ", nextAvailableCell);
        notes[nextAvailableCell] = sensedInput;
        l("Recorded: ", notes[nextAvailableCell]);
        return;
      }
    }
  }
}


void play()
{
  l("supposed to play: ", notes[countNotes]);
  int_array_to_string(notes, sizeof(notes));
  tone(BUZZER_PIN, notes[countNotes], duration);
  countNotes = (countNotes + 1) % MAX_NOTES;
  l(countNotes);
}


void looper()
{
  // Being limited to not create any additional global variables outside of
  // these methods is proving to be quite the challenge...

  // captures the current time
  unsigned long currentMillis = millis();
  // bitmasks the long-cast-to-int for the last 4 bits (1111)
  int noteToPlay = currentMillis & 0x8; // returning a number between 0 and 15
  
  l("note to play: ", noteToPlay);
  l("played ", notes[noteToPlay]);
  tone(BUZZER_PIN, notes[noteToPlay], duration);
  countNotes = (countNotes + 1) % MAX_NOTES;
}

int playnote () {
  int sensedInput = analogRead(NOTE_IN_PIN); // read resistor value
  if (sensedInput > 10) // simple high-pass
  {
    tone(BUZZER_PIN, sensedInput, duration);
    l("played tone: ", sensedInput);
    return sensedInput;
  } else {
    return 0;
  }
}


// UTIL ----------------------------------------------
void playnote (int note) {
  tone(BUZZER_PIN, note, duration);
  l("played tone: ", note);
}

int readnote () {
  int in = analogRead(NOTE_IN_PIN);
  delay(200); // debouncing
  return in;
}
