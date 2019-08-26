
#define signal_pin 11
#define switch_pin A0
#define test_pin A1

void setup() {
  Serial.begin(9600);
  pinMode(signal_pin, OUTPUT);
  pinMode(switch_pin, INPUT);
  pinMode(test_pin, INPUT);

}

void loop() {
//  Serial.println("out of loop - piston stopped");
//  digitalWrite(signal_pin, LOW);
//  Serial.println(analogRead(switch_pin));
//  while (analogRead(switch_pin) > 650){
    Serial.println("in loop - piston should be moving");
    digitalWrite(signal_pin, HIGH);
    //Serial.println(analogRead(test_pin));
    delay(150);
    digitalWrite(signal_pin, LOW);
    //Serial.println(analogRead(test_pin));
    delay(150);
  //}
}
