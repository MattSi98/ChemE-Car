
//initialize pins
int pwm_signal = A5;  //pin signal is sent out over
int photo_pin = A3;   //input from potions voltage divider

//variables 
int duty_cycle_val = 0;
float V_desired = 3.5;  //desired volts 
float V_in = 6.0;       //measured power source input - average over calibration runs 

void setup() {
  pinMode(pwm_signal,OUTPUT);
  pinMode(photo_pin, INPUT);

  Serial.begin(9600);
  Serial.println("started");
}

void loop() {
  int photo_val = analogRead(photo_pin);  
  //Serial.print("photo_val: ");
  //Serial.println(photo_val);

  float photo_voltage = (photo_val*5.0/1024);

  //Serial.print("photo_voltage: ");
  //Serial.println(photo_voltage);

  
  if (photo_voltage < 1.45) {
    //pwm signal set to 0% by breaking out of loop - must reset to start again
    Serial.println("Stopped");
    
    duty_cycle_val = 0;
    analogWrite(pwm_signal,duty_cycle_val);
     
  }else{
    //pwm set to duty cycle 
    duty_cycle_val = int(V_desired*255/V_in);
    
    analogWrite(pwm_signal,duty_cycle_val);
     
  }
  
  Serial.print("duty cycle: ");
  Serial.println(duty_cycle_val);
  delay(50);    //wait 50ms until next update

}
