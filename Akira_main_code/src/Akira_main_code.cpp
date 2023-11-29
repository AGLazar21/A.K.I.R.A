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

SYSTEM_THREAD(ENABLED);
DFRobotDFPlayerMini myDFPlayer;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

int wheresHand();
void musicMode(int handPos);

IoTTimer handTimer1;
IoTTimer handTimer2;
IoTTimer handTimer3;
int handLoc;
bool pausePlay;

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  waitFor(Serial.isConnected,10000);


  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }


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
  if(handLoc){
    musicMode(handLoc);
    
  }
}

void musicMode(int handPos){
  static int timer, curVol;
  static int prevHandPos;
  timer=-9999999;
  
  switch(handPos){
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
        Serial.printf("Hand Position:%i\n",handPos);
        pausePlay = !pausePlay;
        if (pausePlay ==1) {
          if (millis() - timer > 60000) {
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
  VL53L0X_RangingMeasurementData_t measure;
  int handPos;  
 
  lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

  if (measure.RangeStatus != 4) {  // phase failures have incorrect data
    if(measure.RangeMilliMeter <= 100){
      handPos = 1;
    }
    else if(measure.RangeMilliMeter >100 && measure.RangeMilliMeter <=200){
      handPos = 2;
    }
    else if(measure.RangeMilliMeter <310){
      handPos = 3;
    }
  } else {
    handPos = 0;
  } 
  return handPos;
}