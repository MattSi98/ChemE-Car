//PCB Motor Controller 
#include <DRV8835MotorShield.h>
#include <Chrono.h>

//Current Chip
#include <Wire.h>
#include <Adafruit_INA219.h>

//rotary encoder stuff
const int encoderPinA= 0;
const int encoderPinB= 1;
int angle=0;
int encoderPos=0;
int encoderStepsPerRevolution= 20;
boolean encoderALast= LOW; //remembers the previous state of the encoder pin A

//SD card
#include <SPI.h>
#include <SD.h>
File myFile;


#define VIN_PIN 2 //A2
#define POT_PIN 1 //A1
#define STOP_PIN 3 //A3, the photopin
#define SWITCH_PIN 0 //A0
#define LED_PIN 13 //built in LED

#define VMOTOR_DES 3.0 //set desired motor voltage here


#define PWM_MAX 400
#define PWM_SET_MAX 200 //not really sure how this works
#define VMOTOR_MIN 1.5
#define VMOTOR_MAX 4.5


DRV8835MotorShield motors;
Chrono myChrono;

Adafruit_INA219 ina219;

long pwm_on_time = 0;
float vmotor;
int Vin = 0;
int light_on = 0; 
int timer_reset = 0;

long pot_value = 0;
float light_val = 0;

unsigned long curr_time;
float v_input;
float v_to_motor;
float curr_dist;
float curr_vel;

unsigned long prev_time = 0;
float prev_dist = 0;


void setup() {
  pinMode(VIN_PIN, INPUT); //A2 -> Input voltage from power source - VIN_PIN
  pinMode(POT_PIN, INPUT); //A1 -> POT_PIN value
  pinMode(SWITCH_PIN, INPUT); //A0 -> Value of switch, read as either high or low, high is pot, low is gnd/hardcode
  pinMode(STOP_PIN, INPUT); //A3-> "STOPPIN", the photoresistor 

  //rotary encoder pins
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  digitalWrite(encoderPinA, HIGH);
  digitalWrite(encoderPinB, HIGH);
  
  //LED CODE, we should add it eventually
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  
  myChrono.restart();
  Serial.begin(115200);
  Serial.println("Reset");

  //current stuff
  uint32_t currentFrequency;
  ina219.begin();

  //SD stuff
  Serial.print("Initializing SD card...");
  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    return;
  }
  Serial.println("initialization done.");
  //open file
  if (SD.exists("data_csv.txt")){
    // delete the file - remove past data:
    Serial.println("Removing old file...");
    SD.remove("data_csv.txt");
  }
  
  
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
  //current calcs
  float current_mA = 0;
  current_mA = ina219.getCurrent_mA();
  Serial.print("Current:       "); Serial.print(current_mA); Serial.println(" mA");


  // Stopping mechanism
  light_val = analogRead(STOP_PIN)*3.3/1024.0; 
  //Serial.println(light_val);
  if(light_val < 1.7){
      myChrono.stop(); 
      stop_motor();    
    }
  
  Vin = (analogRead(VIN_PIN)); //could be Vin = (analogRead(VIN_PIN +1 ));
 
//idea is that we would never need more than 5V, so pwmsetmax is 200 = 1/2 pwm_max
  pwm_on_time = (PWM_SET_MAX*vmotor*1024)/(5*Vin); //big conversion
  if (pwm_on_time > 400) pwm_on_time = 400;

  Serial.println(pwm_on_time);
  motors.setM2Speed(pwm_on_time);

  //rotary encoder 
  boolean encoderA= digitalRead(encoderPinA);
  if ((encoderALast == HIGH) && (encoderA == LOW)){
    if (digitalRead(encoderPinB) == LOW){
      encoderPos--;
    }else{
      encoderPos++;
    }
    angle= (encoderPos % encoderStepsPerRevolution) * 360/encoderStepsPerRevolution;
    //Serial.println(encoderPos);
    }
    encoderALast= encoderA;

    //current time
    curr_time = millis(); //milliseconds
    //voltage in
    v_input = Vin * 10.0/1024.0;
    //motor voltage
    v_to_motor = v_input * pwm_on_time/400.0;
    //curr dist
    curr_dist = encoderPos/20.0; //feet
    //curr vel
    curr_vel = curr_dist / (float)curr_time *1000;
    
    
    //SD write
    File myFile = SD.open("data_csv.txt", FILE_WRITE);
    if (myFile) {
      Serial.print("Writing to data_csv.txt...");
      //form: time, vin, vmotor, amps, rotPos, dist, vel, acc
      myFile.println(String(curr_time) + "," + String(v_input) + "," + String(v_to_motor) + "," + String(current_mA) + "," + String(encoderPos) + "," + String(curr_dist) + "," + String(curr_vel));
      // close the file:
      myFile.close();
      //Serial.println("done.");
    } else {
      // if the file didn't open, print an error:
      Serial.println("error opening data_csv.txt");
    }

    //prev_time = curr_time;
    //prev_dist = curr_dist;
}

