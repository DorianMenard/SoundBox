/* Play Melody
   -----------

   Program to play a simple melody

   Tones are created by quickly pulsing a speaker on and off
     using PWM, to create signature frequencies.

   Each note has a frequency, created by varying the period of
    vibration, measured in microseconds. We'll use pulse-width
    modulation (PWM) to create that vibration.

   We calculate the pulse-width to be half the period; we pulse
    the speaker HIGH for 'pulse-width' microseconds, then LOW
    for 'pulse-width' microseconds.
    This pulsing creates a vibration of the desired frequency.

   (cleft) 2005 D. Cuartielles for K3
   Refactoring and comments 2006 clay.shirky@nyu.edu
   See NOTES in comments at end for possible improvements
*/

// TONES  ==========================================
// Start by defining the relationship between
//       note, period, &  frequency.
#define  c     3830    // 261 Hz
#define  d     3400    // 294 Hz
#define  e     3038    // 329 Hz
#define  f     2864    // 349 Hz
#define  g     2550    // 392 Hz
#define  a     2272    // 440 Hz
#define  b     2028    // 493 Hz
#define  C     1912    // 523 Hz
// Define a special note, 'R', to represent a rest
#define  R     0

// SETUP ============================================
// Set up speaker on a PWM pin (digital 9, 10 or 11)
int speakerOut = 9;
// Do we want debugging on serial out? 1 for yes, 0 for no
int DEBUG = 1;

const int buttonPin1 = 6;
int buttonState1 = 0;

const int buttonPin2 = 5;
int buttonState2 = 0;

const int buttonPin3 = 4;
int buttonState3 = 0;

const int buttonPin4 = 3;
int buttonState4 = 0;

const int startLoopButton = 2;
int startLoopButtonState = 0;

const int led1 = 13;
const int led2 = 12;
const int led3 = 11;
const int led4 = 10;


int loopMode = 0;
int loopCounter = 0;
int indexInLoop = 0;
int pot = 0;

void setup() {
  pinMode(speakerOut, OUTPUT);

  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(buttonPin3, INPUT);
  pinMode(buttonPin4, INPUT);

  pinMode(startLoopButton, INPUT);

  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  pinMode(led4, OUTPUT);

  if (DEBUG) {
    Serial.begin(9600);
  }
}

// MELODY and TIMING  =======================================
//  melody[] is an array of notes, accompanied by beats[],
//  which sets each note's relative length (higher #, longer note)
int melody[] = { 0, 0 , 0 , 0 };
int notes[] = {  c,  d,  e,  f,  g };
int beats[]  = { 16, 16, 16,  16 };
int MAX_COUNT = sizeof(melody) / 2; // Melody length, for looping.

// Set overall tempo
long tempo = 10000;
// Set length of pause between notes
int pause = 1000;
// Loop variable to increase Rest length
int rest_count = 100; //<-BLETCHEROUS HACK; See NOTES

// Initialize core variables
int tone_ = 0;
int beat = 0;
long duration  = 0;

// PLAY TONE  ==============================================
// Pulse the speaker to play a tone for a particular duration
void playTone() {
  long elapsed_time = 0;
  if (tone_ > 0) { // if this isn't a Rest beat, while the tone has
    //  played less long than 'duration', pulse speaker HIGH and LOW
    while (elapsed_time < duration) {

      digitalWrite(speakerOut, HIGH);
      delayMicroseconds(tone_ / 2);

      // DOWN
      digitalWrite(speakerOut, LOW);
      delayMicroseconds(tone_ / 2);

      // Keep track of how long we pulsed
      elapsed_time += (tone_);
    }
    delay(analogRead(pot));
  }
  else { // Rest beat; loop times delay
    for (int j = 0; j < rest_count; j++) { // See NOTE on rest_count
      // delayMicroseconds(duration);
    }
  }
}

// LET THE WILD RUMPUS BEGIN =============================
void loop() {
  // Set up a counter to pull from melody[] and beats[]
  duration = beat * tempo; // Set up timing

  buttonState1 = digitalRead(buttonPin1);
  buttonState2 = digitalRead(buttonPin2);
  buttonState3 = digitalRead(buttonPin3);
  buttonState4 = digitalRead(buttonPin4);
  startLoopButtonState = digitalRead(startLoopButton);

  if (buttonState1 == LOW) onButtonPress(notes[0]);
  if (buttonState2 == LOW) onButtonPress(notes[1]);
  if (buttonState3 == LOW) onButtonPress(notes[2]);
  if (buttonState4 == LOW) onButtonPress(notes[3]);

  if (startLoopButtonState == LOW) {
    if (loopMode == 0) {
      loopMode = 1;
      Serial.println(loopMode);
    } else {
      loopMode = 0;
      Serial.println(loopMode);
    }
    delay(400);
  }

  if (loopMode == 1) {
    if (loopCounter == 4) {
      loopCounter = 0;
    }
    tone_ = melody[loopCounter];
    beat = beats[0];
    loopCounter++;
    playTone();
  }
}

void turnOnLed() {  
  digitalWrite(led1, HIGH);
  digitalWrite(led2, HIGH);
  delay(5);
  digitalWrite(led1, LOW);
  digitalWrite(led2, LOW);

}

void onButtonPress(int note) {
  if (loopMode == 0) {
    tone_ = note;
    beat = beats[0];
    playTone();
  } else {
    if (indexInLoop == 4) {
      indexInLoop = 0;
    }
    switch(indexInLoop){
      case 0: 
         digitalWrite(led1, HIGH); 
       break; 
     case 1:
        digitalWrite(led2, HIGH); 
    }
    melody[indexInLoop] = note;
    indexInLoop++;
    Serial.println(note);
  }
  delay(analogRead(pot));
}


/*
   NOTES
   The program purports to hold a tone for 'duration' microseconds.
    Lies lies lies! It holds for at least 'duration' microseconds, _plus_
    any overhead created by incremeting elapsed_time (could be in excess of
    3K microseconds) _plus_ overhead of looping and two digitalWrites()

   As a result, a tone of 'duration' plays much more slowly than a rest
    of 'duration.' rest_count creates a loop variable to bring 'rest' beats
    in line with 'tone' beats of the same length.

   rest_count will be affected by chip architecture and speed, as well as
    overhead from any program mods. Past behavior is no guarantee of future
    performance. Your mileage may vary. Light fuse and get away.

   This could use a number of enhancements:
   ADD code to let the programmer specify how many times the melody should
       loop before stopping
   ADD another octave
   MOVE tempo, pause, and rest_count to #define statements
   RE-WRITE to include volume, using analogWrite, as with the second program at
            http://www.arduino.cc/en/Tutorial/PlayMelody
   ADD code to make the tempo settable by pot or other input device
   ADD code to take tempo or volume settable by serial communication
            (Requires 0005 or higher.)
   ADD code to create a tone offset (higer or lower) through pot etc
   REPLACE random melody with opening bars to 'Smoke on the Water'
*/
