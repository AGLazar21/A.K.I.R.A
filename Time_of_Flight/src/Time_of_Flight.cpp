/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include "Adafruit_VL53L0X.h"
SYSTEM_MODE(SEMI_AUTOMATIC);

SYSTEM_THREAD(ENABLED);

Adafruit_VL53L0X lox = Adafruit_VL53L0X();
int handPos();
int handLoc;

void setup() {
  Serial.begin(9600);
  waitFor(Serial.isConnected,10000);

  // wait until serial port opens for native USB devices
  while (! Serial) {
    delay(1);
  }
  
  Serial.println("Adafruit VL53L0X test");
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
  // power 
  Serial.println(F("VL53L0X API Simple Ranging example\n\n")); 
}


void loop() {
  handLoc = handPos();
  Serial.printf("Hand Position:%i\r",handLoc);
}

int handPos(){
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