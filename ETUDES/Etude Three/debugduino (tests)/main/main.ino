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

void l(String = "", String = "", String = "", String = ""); // for ease of debugging

void l(String s, String s2) { Serial.print(s); Serial.println(s2); } // for ease of debugging
void l(String s, int i) { Serial.print(s); Serial.println(i); } // for ease of debugging
void l(int i) { Serial.println(i); } // for ease of debugging
void l(String s, String s2, String s3, String s4) {
  Serial.print(s + ' ');
  Serial.print(s2 + ", ");
  Serial.print(s3 + ", ");
  Serial.println(s4 + '.');
}

void m () {

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
  if (buttonState == 1) {
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
  int sensedInput = readnote();

  if (sensedInput > 10)                      // simple high-pass
  {
    l("note read: ", sensedInput);
    // get the next place in the array to place a note,
    // another artefact of the no-extra-vars rule
    int nextAvailableCell = 0;

    for (int i = 0; i < MAX_NOTES; i++)
    {

      if (i == ( MAX_NOTES - 1 ))
      {
        l("done!");
        // because this is cheaper than implementing a linked list every loop
        memset(notes, 0, sizeof(notes));
        return; // leaving the nextAvailableCell value at zero
      }

      if (notes[i] == 0) {
        return; // stop when index value is uninitialized
      }
      nextAvailableCell += 1;
    }
    l("Recorded: ", sensedInput);
    int_array_to_string(notes, sizeof(notes));
    notes[nextAvailableCell] = sensedInput;
  }
}


void play()
{
  l("supposed to play: ", notes[countNotes]);
  int_array_to_string(notes, sizeof(notes));
  tone(BUZZER_PIN, notes[countNotes], duration);
  countNotes = (countNotes + 1) % MAX_NOTES;
}


void looper()
{
  // Being limited to not create any additional global variables outside of
  // these methods is proving to be quite the challenge
  tone(BUZZER_PIN, notes[countNotes], duration);
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

int readnote() {
  int in = analogRead(NOTE_IN_PIN);
  delay(200);
  return in;
}

void int_array_to_string(int int_array[], int size_of_array)
{
  String returnstring = "";
  for (int temp = 0; temp < size_of_array; temp++)
    returnstring += String(int_array[temp]) + " ";
  Serial.println(returnstring);
}
