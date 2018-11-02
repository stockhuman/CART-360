/**********************ETUDE 2 CART 360 2017*******************************
 * WELCOME! 
 * THE PURPOSE OF THIS EXERCISE IS TO DESIGN A VERY SIMPLE KEYBOARD (5 KEYS)
 * WHICH ALLOWS YOU TO PLAY LIVE, RECORD, AND PLAYBACK SINGLE NOTES (NO CHORDS). 
 * THERE WILL BE A BUTTON WHICH WHEN PRESSED WILL TAKE THE USER TO THE NEXT MODE:
 * THERE ARE 5 POSSIBLE MODES
 * 0 ==  reset    ==   led off == also resets
 * 1 ==  live     ==   led BLUE
 * 2 ==  record   ==   led RED
 * 3 ==  play     ==   led GREEN
 * 4 ==  looper   ==   led PURPLE
 * 
 * ADDITIONALLY - WHEN THE USER CHANGES MODE, 
 * THE RGB LED WILL CHANGE COLOR (ACCORDING TO THE SPECIFICATIONS)

 * PLEASE FOLLOW THE INSTRUCTIONS IN THE COMMENTS:
 * DO NOT ADD ANY MORE FUNCTION DEFINITIONS 
 * ONLY IMPLEMENT THE FUNCTION DEFINITIONS SUPPLIED
 * THERE IS NO NEED TO ADD ANY NEW VARIABLES OR CONSTANTS
 * PROVIDE COMMENTS FOR ANY OF THE CODE IMPLEMENTED
 * GOOD LUCK!
 */

// SEE SHORT ANSWER AT THE BOTTOM
 
/**** CONSTANTS ********************************************************/

#define BUTTON_MODE_PIN 2 // Button to change the mode

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

/******** VARIABLES *****************************************************/
// counter for how many notes we have
int countNotes = 0;
int mode = 0; // start at off
// array to hold the notes played (for record/play mode)
int notes [MAX_NOTES]; 

/*************************************************************************/


/**********************SETUP() DO NOT CHANGE*******************************/
// Declare pin mode for the single digital input
void setup()
{
  pinMode(BUTTON_MODE_PIN, INPUT);
}

/**********************LOOP() DO NOT CHANGE *******************************
 * INSTRUCTIONS: 
 * There is NO NEED to change the loop - it establishes the flow of the program
 * We call here 3 functions repeatedly:
 * 1: chooseMode(): this function will determine the mode that your program is in 
 * based on if the button (linked to the BUTTON_MODE_PIN) was pressed
 * 2: setRGB(): will set the color of the RGB LED based on the value of the mode variable
 * 3: selectMode(): will determine which function to call based on the value of the mode variable

**************************************************************************/
void loop()
{
  chooseMode();
  setRGB();
  selectMode();
}
/******************CHOOSEMODE(): IMPLEMENT *********************************
 * INSTRUCTIONS:
 * Read the value from the Button (linked to the BUTTON_MODE_PIN) and 
 * if is being pressed then change the mode variable.
 * REMEMBER:
 * mode == 0 is reset 
 * mode == 1 is live
 * mode == 2 is record
 * mode == 3 is play
 * mode == 4 is loopMode
 * Every time the user presses the button, the program will go to the next mode,
 * once it reaches 4, it should go back to mode == 0. 
 * (i.e. if mode ==2 and we press, then mode ==3) ...
**************************************************************************/
void chooseMode(){
  // capture the current button state
  int buttonState = digitalRead(BUTTON_MODE_PIN);
  
  // test if the mode switch button is pressed
  if (buttonState == 1) {
    // increment the mode by one, and modulo-wrap it around to 0
    mode = (mode + 1) % 5;
    // debouncing without variables!
    delay(250);
  }
}

/******************SETRGB(): IMPLEMENT *********************************
 * INSTRUCTIONS:
 * Depending on the value of the mode variable:
 * if the mode is 0 - RGB LED IS OFF
 * if the mode is 1 - RGB LED IS BLUE
 * if mode is 2 - RGB LED IS RED
 * if mode is 3 - RGB LED IS GREEN
 * if mode is 4 - RGB LED iS PURPLE
 * YOU MUST USE A SWITCH CASE CONSTRUCT (NOT A SERIES OF IF / ELSE STATEMENTS
**************************************************************************/
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
/**********************SELECTMODE() DO NOT CHANGE *******************************
 * INSTRUCTIONS: 
 * There is NO NEED to change this function - it selects WHICH function should run based on the mode variable
 * There are 4 possibilities
 * 1: reset(): this function will reset any arrays etc, and will do nothing else
 * 2: live(): this function will play the corresponding notes 
 * to the user pressing the respective buttons. 
 * NOTE:: the notes played are NOT stored
 * 3: record(): this function will play the corresponding notes 
 * to the user pressing the respective buttons
 * AND will STORE up to 16 consecutive notes in an array.
 * 4: play(): this function will playback any notes stored in the array that were recorded
 * in the previous mode
 * 5: loopMode(): this function will playback any notes stored in the array that were recorded, 
 * BUT unlike the previous mode, you can choose in which sequence the notes are played.
 * REQUIRED: only play notes from the array (no live stuff)

******************************************************************************/
void selectMode()
{
  if(mode == 0) { 
    reset();
  }
  else if(mode == 1) {
    live();
  }
  else if(mode == 2) {
    record();
  }
  
  else if(mode == 3) {
    play();
  }
   
   else if(mode == 4) {
    looper();
  }
}
/******************RESET(): IMPLEMENT **************************************
 * INSTRUCTIONS:
 * this function should ensure that any notes recorded are no longer available
**************************************************************************/
void reset()
{
  // the performance implications of zeroing out memory on every iteration of
  // loop() seems excessive.
  memset(notes, 0, sizeof(notes));
  delay(30); // tiny pause, for the health of the chip. :)
}
/******************LIVE(): IMPLEMENT **************************************
 * INSTRUCTIONS:
 * this function will play the corresponding notes 
 * to the user pressing the respective buttons
 * NOTE:: the notes played are NOT stored
 * SO: you need read in the input from the analog input (linked to the button-resistor ladder combo)
 * THEN - output the note to the buzzer using the tone() function
**************************************************************************/
void live()
{
  int note = analogRead(NOTE_IN_PIN);
  delay(200); // debouncing
  tone(BUZZER_PIN, note, duration);
}
/******************RECORD(): IMPLEMENT **********************************
 * INSTRUCTIONS:
 * this function will play the corresponding notes 
 * to the user pressing the respective buttons
 * AND will STORE up to 16 consecutive notes in an array.
 * SO:you need read in the input from the analog input (linked to the button-resistor ladder combo)
 * AND - output the note to the buzzer using the tone() function
 * THEN store that note in the array  - BE CAREFUL - you can only allow for up to MAX_NOTES to be stored
**************************************************************************/
void record()
{
  int sensedInput = analogRead(NOTE_IN_PIN); // capture the current note, if any
  delay(150); // debouncing

  if (sensedInput > 10) { // simple high-pass

    // get the next place in the array to place a note,
    // another artefact of the no-extra-vars rule.

    // I initially used an iterator variable, but I saw that simply integrating
    // my iterator with this nextAvailableCell var was more concise and expressive
    for (int nextAvailableCell = 0; nextAvailableCell < MAX_NOTES; nextAvailableCell++)
    {
      // bail early if we're at the limit of the tune
      if ( nextAvailableCell == ( MAX_NOTES - 1 ))
      {
        // because this is cheaper than implementing a linked list every loop
        memset(notes, 0, sizeof(notes));
        notes[0] = sensedInput; // write to the beggining of the array
        return;
      }

      // test if current note is uninitialised
      if (notes[nextAvailableCell] == 0) {
        notes[nextAvailableCell] = sensedInput;
        return;
      }
    }
  }
}
/******************PLAY(): IMPLEMENT ************************************
 * INSTRUCTIONS:
 * this function will playback any notes stored in the array that were recorded
 * in the previous mode
 * SO: you need to go through the array of values (be careful - the user may not have put in MAX_NOTES)
 * READ each value IN ORDER
 * AND output each note to the buzzer using the tone() function
 * ALSO: as long as we are in this mode, the notes are played over and over again
 * BE CAREFUL: make sure you allow for the user to get to another mode from the mode button...
**************************************************************************/
void play()
{
  tone(BUZZER_PIN, notes[countNotes], duration);
  countNotes = (countNotes + 1) % MAX_NOTES;
}
/******************LOOPMODE(): IMPLEMENT *********************************
 * INSTRUCTIONS:
 * this function will playback any notes stored in the array that were recorded
 * in the previous mode
 * SO: you need to go through the array of values (be careful - the user may not have put in MAX_NOTES)
 * READ values IN ANY ORDERING (You MUST use the array - but you can determine your own sequence)
 * AND output each note to the buzzer using the tone() function
 * ALSO: as long as we are in this mode, the notes are played over and over again
 * BE CAREFUL: make sure you allow for the user to get to another mode from the mode button...
**************************************************************************/
void looper()
{
  // Being limited to not create any additional global variables outside of
  // these methods is proving to be quite the challenge...

  // captures the current time
  unsigned long currentMillis = millis();
  // bitmasks the long-cast-to-int for the last 4 bits (1111)
  int noteToPlay = currentMillis & 0x8; // returning a number between 0 and 15
  
  tone(BUZZER_PIN, notes[noteToPlay], duration);
  countNotes = (countNotes + 1) % MAX_NOTES;
}

/**************************************************************************/



/**
 * the resistor ladder (keyboard), mode selector and what is occurring on the Arduino as a voltage.
 * How does the input become audible sound?
 * --
 * 
 * The resistor ladder functions as such:
 * Much like a number represented in binary, each node in the ladder adds to those after it.
 * Voltage travels through the ladder, with the output voltage ever smaller (following 1 / (I * R))
 * as the resistors are arranged in series. When one presses a button, the circuit completes at that node,
 * and current will flow through each resistor preceeding that node and ultimately end up in the Arduino.
 * 
 * With this we derive the proportional voltage drop over the Arduino's initial 5V out.
 * By feeding these values as PWM duty cycles to the Arduino tone() function, one can produce the 'tone'
 * associated to each resistor ladder. The function takes a frequency as an argument, and then modulates 
 * an output signal that is easily reproduced in the diaphram of the piezo speaker.
 * 
 * The nature of the ladder makes it so that playing chords is impossible, as each node will close the circuit and 
 * render following nodes inert, yet this is sometimes useful for holding a root note and quickly 
 * jumping to notes closer to the voltage source on the ladder.
 * 
 */
