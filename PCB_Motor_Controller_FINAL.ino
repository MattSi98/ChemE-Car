//PCB Motor Controller Version
#include <DRV8835MotorShield.h>
#include <Chrono.h>

#define VIN_PIN 2 //A2
#define POT_PIN 1 //A1
#define STOP_PIN 3 //A3, the photopin
#define SWITCH_PIN 0 //A0
#define LED_PIN 13 //built in LED

#define VMOTOR_DES 3.5 //set desired motor voltage here


#define PWM_MAX 400
#define PWM_SET_MAX 200 //not really sure how this works
#define VMOTOR_MIN 1.5
#define VMOTOR_MAX 4.5


DRV8835MotorShield motors;
Chrono myChrono;


long pwm_on_time = 0;
float vmotor;
int Vin = 0;
int light_on = 0; 
int timer_reset = 0;

long pot_value = 0;
float light_val = 0;

void setup() {
  pinMode(VIN_PIN, INPUT); //A2 -> Input voltage from power source - VIN_PIN
  pinMode(POT_PIN, INPUT); //A1 -> POT_PIN value
  pinMode(SWITCH_PIN, INPUT); //A0 -> Value of switch, read as either high or low, high is pot, low is gnd/hardcode
  pinMode(STOP_PIN, INPUT); //A3-> "STOPPIN", the photoresistor 
  
  //LED CODE, we should add it eventually
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  myChrono.restart();
  Serial.begin(9600);
  Serial.println("Reset");
  
  //efficiency offset
  vmotor = VMOTOR_DES;
}

void stop_motor() {
  Serial.print("Number of milliseconds elapsed: "); 
  Serial.println(myChrono.elapsed());
  
  while(light_val < 2.75){
    pwm_on_time = 0;
    motors.setM2Speed(pwm_on_time);
    delay(2);

    digitalWrite(LED_PIN, LOW); 

//    //if the potions reaction screwed up and turns clear again, start motor again
//    light_val = analogRead(STOP_PIN);
//    if (light_val >= 2.75) {
//      digitalWrite(LED_PIN, HIGH);
//      loop();
//    }
  }
}

void loop() {
  // Stopping mechanism
  light_val = analogRead(STOP_PIN)*3.3/1024.0; 
  //Serial.println(light_val);
  if(light_val < 1.7){
      myChrono.stop(); 
      stop_motor();    
    }
  
  Vin = (analogRead(VIN_PIN)); //could be Vin = (analogRead(VIN_PIN +1 ));
  Serial.print("Vin: ");
  Serial.println(Vin * 10.0 / 1024.0);

  
//idea is that we would never need more than 5V, so pwmsetmax is 200 = 1/2 pwm_max
  pwm_on_time = (PWM_SET_MAX*vmotor*1024)/(5*Vin); //big conversion
  if (pwm_on_time > 400) pwm_on_time = 400;

  //motors.setM1Speed(200); //use for testing - doesnt use pwm - sets motor to one constant duty cycle 0-400
  //Serial.println("Currently running the motor");
  //Serial.print("V_to_motor: ");
  //Serial.println(pwm_on_time * (Vin*5.0/400.0*1024.0));
  Serial.println(pwm_on_time);
  motors.setM2Speed(pwm_on_time);
  delay(50);
}

