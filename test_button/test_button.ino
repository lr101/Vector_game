#define LEFT_PIN   3
#define RIGHT_PIN   2
#define MODE_PIN   12
#define RESET_PIN   13


int userPosition = 0;
int alienPosition = 0;


boolean leftState = LOW;
boolean rightState = LOW;
boolean modeState = LOW;
boolean stateReset = HIGH;
int mode     = 0;    // Currently-active animation mode, 0-9

void setup() {
  pinMode(LEFT_PIN, INPUT);
  pinMode(RIGHT_PIN, INPUT);
  pinMode(MODE_PIN, INPUT);
  pinMode(RESET_PIN, INPUT);
  Serial.begin(9600);
  Serial.print("test");
}

void loop() {
  // Get current button state.
  boolean leftStateNew = digitalRead(LEFT_PIN);
  boolean rightStateNew = digitalRead(RIGHT_PIN);
  boolean modeStateNew = digitalRead(MODE_PIN);
  boolean stateResetNew = digitalRead(RESET_PIN);
  delay(1000);
  Serial.println(leftStateNew);

  // Check if state changed from high to low (button press).
  leftState = checkChangeState(leftStateNew, leftState);
  rightState = checkChangeState(rightStateNew, rightState);
  modeState = checkChangeState(modeStateNew, modeState);

}


bool checkChangeState(boolean newState, boolean oldState) {
  if((newState == HIGH) && (oldState == LOW)) {
    Serial.print("State is high");
  }
  return newState;
}
