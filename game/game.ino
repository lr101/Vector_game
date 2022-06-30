#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif
#include "ArdoinoQueue.h"

#define LEFT_PIN   13
#define RIGHT_PIN   4
#define MODE_PIN   7
#define RESET_PIN   8
#define LED   12
int alienAcc = 0;
#define PIXEL_PIN    6  // Digital IO pin connected to the NeoPixels.

#define PIXEL_COUNT 84  // Number of NeoPixelsf

int NUM_HORIZONTAL  = 14;
int NUM_VERTICAL = 6;
ArduinoQueue<int> intQueue(84);

int dead[] = {14,28,42,56,70,15,30,44,58,71,  18,19,32,46,47,60,74,75, 22,35,37,49,50,51,63,65,77,79,  25,26,39,41,53,55,67,69,81,82};

struct Direction {
  bool right = true;
  bool down = true;

};

Direction pp[PIXEL_COUNT];
int userPosition = 0;
int alienPosition = 57;
unsigned long alienTime;
unsigned long userTime;

// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(PIXEL_COUNT, PIXEL_PIN, NEO_GRB + NEO_KHZ800);

boolean leftState = HIGH;
boolean rightState = HIGH;
boolean modeState = HIGH;
boolean stateReset = HIGH;
boolean mode  = false;    // Currently-active animation mode, 0-9

void setup() {
  pinMode(LEFT_PIN, INPUT_PULLUP);
  pinMode(RIGHT_PIN, INPUT_PULLUP);
  pinMode(MODE_PIN, INPUT_PULLUP);
  pinMode(RESET_PIN, INPUT);
  pinMode(LED,OUTPUT);
  strip.begin(); // Initialize NeoPixel strip object (REQUIRED)
  strip.show();  // Initialize all pixels to 'off'
  Serial.begin(9600);
  mazeSetup1();
  resetMap();
}

void loop() {
  long timeNow = millis();
  if (userTime + 250 < timeNow) {
    boolean leftStateNew = digitalRead(LEFT_PIN);
    boolean rightStateNew = digitalRead(RIGHT_PIN);
    boolean modeStateNew = digitalRead(MODE_PIN);
    boolean stateResetNew = digitalRead(RESET_PIN);
   
    // Check if state changed from high to low (button press).
    if (checkChangeState(modeStateNew, modeState)) {
      mode = !mode;
      if(mode) {
        digitalWrite(LED,HIGH);
      } else {
        digitalWrite(LED,LOW);
      }
      userTime = millis();
    } else {
      if (checkChangeState(leftStateNew, leftState)) {
        if (mode) {
          if (goUp(userPosition)) {
            strip.setPixelColor(userPosition, strip.Color(0,  150,   0));
            userPosition -= 14;
            updatePosition();
          }
        } else {
          if (goLeft(userPosition)) {
            strip.setPixelColor(userPosition, strip.Color(0, 150,   0));
            userPosition -= 1;
            updatePosition();
          }
        }
      }
      if (checkChangeState(rightStateNew, rightState)) {
        if (mode) {
          if (goDown(userPosition)) {
            strip.setPixelColor(userPosition, strip.Color(0, 150,   0));
            userPosition += 14;
            updatePosition();
          }
        } else {
          if (goRight(userPosition)) {
            strip.setPixelColor(userPosition, strip.Color(0, 150,   0));
            userPosition += 1;
            updatePosition();
          }
        }
      }
    } 
  }
  checkAlien();
  
}

void checkGameOver() {
  if (alienPosition == userPosition) {
    writeDead();
    strip.setPixelColor(0, strip.Color(0,0,0)); 
    strip.setPixelColor(1, strip.Color(0,0,0)); 
    strip.setPixelColor(2, strip.Color(0,0,0)); 
    strip.show();
    delay(5000);
    resetMap();
  }
}

void resetMap() {
  while(!intQueue.isEmpty()) {
    intQueue.dequeue();
  }
  userPosition = 0;
  alienPosition = 57;
  mode  = false;
  alienAcc = 0;
  digitalWrite(LED,LOW);
  for(int i=0; i<PIXEL_COUNT; i++) { // For each pixel...
    strip.setPixelColor(i, strip.Color(0, 150, 0));
  }
  alienTime = millis();
  userTime = millis();
  intQueue.enqueue(43);
  intQueue.enqueue(29);
  intQueue.enqueue(28);
  intQueue.enqueue(14);
  intQueue.enqueue(0);
  strip.setPixelColor(userPosition,strip.Color(255,182,193));
  strip.setPixelColor(alienPosition,strip.Color(255,0,0));
  strip.setPixelColor(68,strip.Color(0,0,255));
  strip.setPixelColor(69,strip.Color(0,0,255));
  strip.setPixelColor(82,strip.Color(0,0,255));
  strip.setPixelColor(83,strip.Color(0,0,255));
  strip.show();   // Send the updated pixel colors to the hardware.
  
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i< PIXEL_COUNT; i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void writeDead() {
  colorWipe(strip.Color(0,0,0), 1);
  for(int i = 0; i < sizeof(dead); i++) {
     strip.setPixelColor(dead[i], strip.Color(255,0,0));         //  Set pixel's color (in RAM)
     strip.show();                          //  Update strip to match
     delay(1);
  }
}

void rainbow(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 3*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}

void checkAlien() {
  long timeNow = millis();
  // 
  if (alienTime + 2000- (alienAcc*40) < timeNow) {
     alienTime = timeNow;
     updateAlienPosition();
     checkGameOver();
  }
}

void updateAlienPosition() {
   strip.setPixelColor(alienPosition,strip.Color(0,   150,   0));
   alienPosition = intQueue.dequeue();
   strip.setPixelColor(alienPosition,strip.Color(255,   0,   0));
   strip.show();
}

void updatePosition(){
  alienAcc++;
  int alienAcc = 0;
  intQueue.enqueue(userPosition);
  strip.setPixelColor(userPosition,strip.Color(255,182,193));
  strip.show();
  userTime = millis();
  if (userPosition == 68) {
    rainbow(1);
    delay(5000);
    resetMap();
  }
}

bool checkChangeState(boolean newState, boolean oldState) {
  if((newState == LOW) && (oldState == HIGH)) {
    return true;
  }
  return false;
}

void mazeSetup1() {
  pp[1].down = false;
  pp[2].down = false;
  pp[4].right = false;
  pp[6].down = false;
  pp[7].down = false;
  pp[8].down = false;
  pp[10].down = false;
  pp[12].right = false;
  //Reihe 2
  pp[15].down = false;
  pp[17].down = false;
  pp[21].down = false;
  pp[22].down = false;
  pp[25].down = false;
  pp[24].down = false;
  pp[15].right = false;
  pp[17].right = false;
  pp[18].right = false;
  pp[19].right = false;
  pp[23].right = false;
  pp[25].right = false;
  pp[26].right = false;
  //Reihe 3
  pp[28].down = false;
  pp[31].down = false;
  pp[32].down = false;
  pp[34].down = false;
  pp[36].down = false;
  pp[37].down = false;
  pp[40].down = false;
  pp[29].right = false;
  pp[30].right = false;
  pp[32].right = false;
  pp[33].right = false;
  pp[34].right = false;
  pp[37].right = false; 
  pp[39].right = false;
  //Reihe 4
  pp[44].down = false;
  pp[45].down = false;
  pp[47].down = false;
  pp[51].down = false;
  pp[55].down = false;
  pp[42].right = false;
  pp[43].right = false;
  pp[46].right = false;
  pp[48].right = false;
  pp[49].right = false;
  pp[52].right = false;
  pp[54].right = false;
  //Reihe 5
  pp[57].down = false;
  pp[58].down = false;
  pp[60].down = false;
  pp[61].down = false;
  pp[62].down = false;
  pp[64].down = false;
  pp[65].down = false;
  pp[56].right = false;
  pp[57].right = false;
  pp[59].right = false;
  pp[62].right = false;
  pp[63].right = false;
  pp[65].right = false;
  pp[66].right = false;
  pp[67].right = false;
  //Reihe
  pp[80].right = false;
  pp[81].right = false;
}


/**
* returns true if it is possible to go left from the given {position}
*/
bool goLeft(int position) {
  if (position % NUM_HORIZONTAL == 0) {
    return false;
  } else {
    return goRight(position - 1);
  }
}


bool goRight(int position) {
  if (pp[position].right) {
    if (position % NUM_HORIZONTAL == 13) {
        return false;
    } else {
      return true;
    }
  } else {
    return false;
  }
}


bool goUp(int position) {
  if (position / NUM_HORIZONTAL == 0) {
    return false;
  } else {
    return goDown(position - 14);
  }
}


bool goDown(int position) {
  if (pp[position].down) {
    if (position / NUM_HORIZONTAL == 5) {
      return false;
    } else {
      return true;
    }
  } else {
    return false;
  }
}
