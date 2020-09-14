//PCB Motor Controller Version
#include <DRV8835MotorShield.h>
#include <Chrono.h>

#define VIN_PIN 2 //A2
#define POT_PIN 1 //A1
#define STOP_PIN 3 //A3, the photopin
#define SWITCH_PIN 0 //A0
#define LED_PIN 13 //built in LED

#define VMOTOR_DES 3.25 //set desired motor voltage here


#define PWM_MAX 400
#define PWM_SET_MAX 200 //not really sure how this works
#define VMOTOR_MIN 1.5
#define VMOTOR_MAX 4.5


DRV8835MotorShield motors;
Chrono myChrono;

//define variable
unsigned long startTime;
unsigned long stopTime;
unsigned long desiredTime;

long pwm_on_time = 0;
float vmotor;
int Vin = 0;
int light_on = 0; 
int timer_reset = 0;

int pot_value = 0;  //could be long
int light_val = 0;  //could be long



void setup() {
  pinMode(VIN_PIN, INPUT); //A2 -> Input voltage from power source - VIN_PIN
  pinMode(POT_PIN, INPUT); //A1 -> POT_PIN value
  pinMode(SWITCH_PIN, INPUT); //A0 -> Value of switch, read as either high or low, high is pot, low is gnd/hardcode
  pinMode(STOP_PIN, INPUT); //A3-> "STOPPIN", the photoresistor 

  
  //set the desired time (ms)
  desiredTime=80000;
  
  //LED CODE, we should add it eventually
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  myChrono.restart();
  Serial.begin(9600);
  
  //toggle switch code: 
  if (analogRead(SWITCH_PIN)> 650) {  //if switch is to the right
    //Serial.println("in loop");
    pot_value = analogRead(POT_PIN);
    //Serial.println(pot_value);
    vmotor = 1.5 + (pot_value*3.0)/682; //threshhold voltage is 1.5 to get the car to move, lets you be more precise on the pot, improves resolution
    //Serial.println(vmotor);
  }
  else {
    vmotor = VMOTOR_DES;
  }
  //efficiency offset
  vmotor = vmotor + 0.2;
}

void stop_motor() {
  Serial.print("Number of seconds elapsed: "); 
  Serial.println(myChrono.elapsed());
  
  while(light_val < 2.75){
    pwm_on_time = 0;
    motors.setM2Speed(pwm_on_time);
    delay(2);

    digitalWrite(LED_PIN, LOW); 

    //if the potions reaction screwed up and turns clear again, start motor again
    //light_val = analogRead(STOP_PIN);
    //if (light_val >= 2.75) {
      //digitalWrite(LED_PIN, HIGH);
      //loop();
    //}
  }
}

void loop() {
  // Stopping mechanism
  light_val = analogRead(STOP_PIN)*5/1024; 
  if(light_val < 1.7){
      myChrono.stop(); 
      stop_motor();    
    }
  
  Vin = (analogRead(VIN_PIN)); //could be Vin = (analogRead(VIN_PIN +1 ));
  //Serial.print("current Vin: ");
  //Serial.println(Vin);
  
  //Serial.print("potpin: ");
  //Serial.println(pot_value);
  //Serial.print("vmotor: ");
  //Serial.println(vmotor);
  
  //idea is that we would never need more than 5V, so pwmsetmax is 200 = 1/2 pwm_max
  while (millis()<=desiredTime) {
    pwm_on_time = (PWM_SET_MAX*vmotor*1024)/(5*Vin); //big conversion
    Serial.println(pwm_on_time);
    if (pwm_on_time > 400){
      pwm_on_time = 400;
    }
    motors.setM2Speed(pwm_on_time);
  }
  motors.setM2Speed(0);
  delay(50);
  }




