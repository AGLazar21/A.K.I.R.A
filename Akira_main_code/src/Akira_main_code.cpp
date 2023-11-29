/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "DFRobotDFPlayerMini.h"
#include "Adafruit_VL53L0X.h"
#include "IoTTimer.h"
SYSTEM_MODE(SEMI_AUTOMATIC);

DFRobotDFPlayerMini myDFPlayer;
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox3 = Adafruit_VL53L0X();

int wheresHand();
void setID();
void musicMode(int handPos);

IoTTimer handTimer1;
IoTTimer handTimer2;
IoTTimer handTimer3;
int handLoc;
bool pausePlay;

int LOX1_ADDRESS = 0x30;
int LOX2_ADDRESS = 0x31;
int LOX3_ADDRESS = 0x32;
int SHT_LOX1 = D5;
int SHT_LOX2 = D6;
int SHT_LOX3 = D7;
VL53L0X_RangingMeasurementData_t measure1;
VL53L0X_RangingMeasurementData_t measure2;
VL53L0X_RangingMeasurementData_t measure3;



void setID() {
  // all reset
  digitalWrite(SHT_LOX1, LOW);    
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);
  delay(10);
  // all unreset
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, HIGH);
  digitalWrite(SHT_LOX3, HIGH);
  delay(10);

  // activating LOX1 and reseting LOX2
  digitalWrite(SHT_LOX1, HIGH);
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);

  delay(10);
  if (!lox1.begin(LOX1_ADDRESS)) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }

  digitalWrite(SHT_LOX2, HIGH);
  // initing LOX1
  if(!lox2.begin(LOX2_ADDRESS)) {
    Serial.println(F("Failed to boot second VL53L0X"));
    while(1);
  }
  delay(10);

  // activating LOX2
  digitalWrite(SHT_LOX3, HIGH);
  delay(10);

  //initing LOX2
  if(!lox3.begin(LOX3_ADDRESS)) {
    Serial.println(F("Failed to boot third VL53L0X"));
    while(1);
  }
  
}


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  waitFor(Serial.isConnected,10000);


  while (! Serial) {
    delay(1);
  }
  
  pinMode(SHT_LOX1, OUTPUT);
  pinMode(SHT_LOX2, OUTPUT);
  pinMode(SHT_LOX3, OUTPUT);

  Serial.println("Shutdown pins inited...");

  digitalWrite(SHT_LOX1, LOW);
  digitalWrite(SHT_LOX2, LOW);
  digitalWrite(SHT_LOX3, LOW);

  Serial.println("Both in reset mode...(all 3 are low)");
  
  
  Serial.println("Starting...");
  setID();


 if (!myDFPlayer.begin(Serial1)) {  //Use softwareSerial to communicate with mp3.
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.play(1);  //Play the first mp3
  myDFPlayer.volume(15); 
}


void loop() {
  handLoc = wheresHand();
  Serial.printf("Hand Position: %i\n",handLoc);
  /* if(handLoc){
    musicMode(handLoc);
    
  } */
}

void musicMode(int handPos){
  static int timer, curVol;
  static int prevHandPos;
  timer=-9999999;
  
  switch(handPos){
    case 0:
      prevHandPos =0;
      break;
    case 3:
      if(handPos != prevHandPos){
        handTimer3.startTimer(1000);
      }
      if(handTimer3.isTimerReady()){
        Serial.printf("Hand Position:%i\n",handPos);
        myDFPlayer.volumeUp();
        handTimer3.startTimer(500);
        curVol = myDFPlayer.readVolume();
        Serial.printf("Vol:%i",curVol);
      }
      prevHandPos = handPos;
      break;
    /* case 4:
      myDFPlayer.previuos();
      break; */
    case 2:
      if(handPos != prevHandPos){
        handTimer2.startTimer(1000);
      }
      if(handTimer2.isTimerReady()){
        handTimer2.startTimer(500000);
        Serial.printf("Hand Position:%i\n",handPos);
        pausePlay = !pausePlay;
        Serial.printf("pausePlay:%i\n",pausePlay);
        if (pausePlay ==1) {
          if (millis() - timer > 60000000) {
            timer = millis();
            myDFPlayer.next();  //Play next mp3 every 3 second.
          }
        }
        else{
          myDFPlayer.pause();
        }
      }
      prevHandPos = handPos;
      break;
   /*  case 6:
      myDFPlayer.next();
      break; */
    case 1:
      if(handPos != prevHandPos){
        handTimer1.startTimer(1000);
      }
      if(handTimer1.isTimerReady()){
        Serial.printf("Hand Position:%i\n",handPos);
        myDFPlayer.volumeDown();
        handTimer1.startTimer(500);
        curVol = myDFPlayer.readVolume();
        Serial.printf("Vol:%i",curVol);
      }
      prevHandPos = handPos;
      
      break;
  }
}

int wheresHand(){
  int handPos;  
 
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!

  if (measure1.RangeStatus != 4) {  // phase failures have incorrect data
    if(measure1.RangeMilliMeter <= 100){
      handPos = 7;
    }
    else if(measure1.RangeMilliMeter >100 && measure1.RangeMilliMeter <=200){
      handPos = 4;
    }
    else if(measure1.RangeMilliMeter >200 && measure1.RangeMilliMeter <=320){
      handPos = 1;
    }
  } else {
    handPos = 0;
  } 
if(handPos == 0 ){
  lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!

  if (measure2.RangeStatus != 4) {  // phase failures have incorrect data
    if(measure2.RangeMilliMeter <= 100){
      handPos = 8;
    }
    else if(measure2.RangeMilliMeter >100 && measure2.RangeMilliMeter <=200){
      handPos = 5;
    }
    else if(measure2.RangeMilliMeter >200 && measure2.RangeMilliMeter <=320){
      handPos = 2;
    }
  } else {
    handPos = 0;
  } 
}
if(handPos == 0){
  lox3.rangingTest(&measure3, false); // pass in 'true' to get debug data printout!

  if (measure3.RangeStatus != 4) {  // phase failures have incorrect data
    if(measure3.RangeMilliMeter <= 100){
      handPos = 9;
    }
    else if(measure3.RangeMilliMeter >100 && measure3.RangeMilliMeter <=200){
      handPos = 6;
    }
    else if(measure3.RangeMilliMeter >200 && measure3.RangeMilliMeter <=320){
      handPos = 3;
    }
  } else {
    handPos = 0;
  } 
}
return handPos;
}