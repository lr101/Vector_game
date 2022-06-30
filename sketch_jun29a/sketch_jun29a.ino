#define LEFT_PIN   3
void setup() {
  pinMode(LEFT_PIN, INPUT);
  digitalWrite(LEFT_PIN,LOW);
  Serial.begin(9600);
  Serial.print("test");
  // put your setup code here, to run once:

}

void loop() {
  Serial.println(digitalRead(LEFT_PIN));
  delay(100);
  // put your main code here, to run repeatedly:

}
