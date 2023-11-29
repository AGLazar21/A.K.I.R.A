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
SYSTEM_MODE(SEMI_AUTOMATIC);

SYSTEM_THREAD(ENABLED);
DFRobotDFPlayerMini myDFPlayer;
Adafruit_VL53L0X lox = Adafruit_VL53L0X();
int wheresHand();
void musicMode(int handPos);
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

}


void loop() {
  handLoc = wheresHand();
  if(handLoc){
    musicMode(handLoc);
    Serial.printf("Hand Position:%i\n",handLoc);
  }
}

void musicMode(int handPos){
  static int timer;
  myDFPlayer.volume(20); 
  switch(handPos){
    case 3:
      myDFPlayer.volumeUp();
      Serial,printf("Vol:%i",myDFPlayer.volume());
      break;
    /* case 4:
      myDFPlayer.previuos();
      break; */
    case 2:
      pausePlay = !pausePlay;
      timer=-9999999;
      if (pausePlay ==1) {
        if (millis() - timer > 60000) {
          timer = millis();
          myDFPlayer.next();  //Play next mp3 every 3 second.
        }
      }
      else{
        myDFPlayer.pause();
      }
      break;
   /*  case 6:
      myDFPlayer.next();
      break; */
    case 1:
      myDFPlayer.volumeDown();
      Serial,printf("Vol:%i",myDFPlayer.volume());
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