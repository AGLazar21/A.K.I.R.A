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
#include "Colors.h"
#include <neopixel.h>
#include "hue.h"
#include "Button.h"
SYSTEM_MODE(MANUAL);

Button Button(D19);

const int BULB=4; 

const int PIXELCOUNT = 4;
Adafruit_NeoPixel pixel(PIXELCOUNT,SPI1,WS2812B);
void pixelFill(int pixelNum, int pixColor);



DFRobotDFPlayerMini myDFPlayer;
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox3 = Adafruit_VL53L0X();

int wheresHand();
void setID();
void musicMode(int handPos);
void lightMode(int handPos);

IoTTimer handTimer;

static int handLoc, curMode;
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
  Serial.printf("All Connected\n");
}


void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);
  waitFor(Serial.isConnected,10000);

  WiFi.on();
  WiFi.clearCredentials();
  WiFi.setCredentials("IoTNetwork");
  WiFi.connect();
  while(WiFi.connecting()) {
    Serial.printf(".");
  }
  Serial.printf("\n\n");

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
  
  myDFPlayer.randomAll();  //Play the first mp3
  pausePlay = 1;
  myDFPlayer.volume(15); 


  pixel.begin();
  pixel.setBrightness(30);
  pixel.show();

  curMode = 0;


}


void loop() {
  handLoc = wheresHand();
  if(Button.isClicked()){
    curMode = (curMode+1)%4;
    Serial.printf("Mode: %i\n",curMode);
  }
  switch(curMode){
    case 0:
    pixelFill(3,black);
      break;
    case 1:
    pixelFill(3,blue);
    if(handLoc){
      musicMode(handLoc);
    }
      break;
    case 2:
    pixelFill(3,orange);
    if(handLoc){
      lightMode(handLoc);
    }
      break;
    case 3:
    pixelFill(3,purple);
    if(handLoc){
      //tempMode();
    }
      break;
  }
} 
void musicMode(int handPos){
 static int curVol; //,curState;
  static int prevHandPos;
  
  switch(handPos){
    case 0:
      prevHandPos =0;
      break;
  /*   case 1: 
     if(handPos != prevHandPos){
        handTimer3.startTimer(1000);
      }
      if(handTimer3.isTimerReady()){
        Serial.printf("Hand Position:%i\n",handPos);
        myDFPlayer.pause();
        curState = myDFPlayer.readState();
        Serial.printf("Current State: %i\r",curState);
        handTimer3.startTimer(500);
      }
      prevHandPos = handPos;
      break;

    case 3: 
     if(handPos != prevHandPos){
        handTimer3.startTimer(1000);
      }
      if(handTimer3.isTimerReady()){
        Serial.printf("Hand Position:%i\n",handPos);
        myDFPlayer.start();
        curState = myDFPlayer.readState();
        Serial.printf("Current State: %i\r",curState);
        handTimer3.startTimer(500);
      }
      prevHandPos = handPos;
      break;
 */
    case 2:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        Serial.printf("Hand Position:%i\n",handPos);
        myDFPlayer.volumeUp();
        handTimer.startTimer(1000);
        curVol = myDFPlayer.readVolume();
        Serial.printf("Vol:%i",curVol);
      }
      prevHandPos = handPos;
      break;

    case 4:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        Serial.printf("Hand Position:%i\n",handPos);
        myDFPlayer.previous();
        handTimer.startTimer(1000);
      }
      prevHandPos = handPos;
      break;

    case 5:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        handTimer.startTimer(1000);
        Serial.printf("Hand Position:%i\n",handPos);
        pausePlay = !pausePlay;
        Serial.printf("pausePlay:%i\n",pausePlay);
        if (pausePlay ==1 && myDFPlayer.readState() == 514){
            myDFPlayer.start();  
        }
        else if(pausePlay == 0 && myDFPlayer.readState() == 513){
          myDFPlayer.pause();
        }
      }
      prevHandPos = handPos;
      break;

    case 6:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        Serial.printf("Hand Position:%i\n",handPos);
        myDFPlayer.next();
        handTimer.startTimer(1000);
      }
      prevHandPos = handPos;
      break;

    case 8:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        Serial.printf("Hand Position:%i\n",handPos);
        myDFPlayer.volumeDown();
        handTimer.startTimer(1000);
        curVol = myDFPlayer.readVolume();
        Serial.printf("Vol:%i",curVol);
      }
      prevHandPos = handPos;
      
      break;
  }
}


void lightMode(int handPos){
  static int prevHandPos, hueBrit=125, hueColor;
  static bool onOff = 0;
  switch(handPos){
    case 0:
      prevHandPos =0;
      break;
    case 2:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        hueBrit = hueBrit+25;
        setHue(BULB,onOff,HueRainbow[hueColor%7],hueBrit,255);
        Serial.printf("Hand Position: %i\n",handPos);
        handTimer.startTimer(1000);
      }
      prevHandPos = handPos;
      break;

    case 4:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        if(hueColor>0){
          hueColor = hueColor-1;
        }
        setHue(BULB,onOff,HueRainbow[hueColor%7],hueBrit,255);
        Serial.printf("Hand Position: %i\n",handPos);
        handTimer.startTimer(1000);
      }
      prevHandPos = handPos;
      break;

    case 5:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        onOff = !onOff;  
        setHue(BULB,onOff,HueRainbow[hueColor%7],hueBrit,255);
        Serial.printf("Hand Position: %i\n",handPos);
        handTimer.startTimer(1000);
      }
      prevHandPos = handPos;
      break;

    case 6:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
          hueColor = hueColor+1;
          setHue(BULB,onOff,HueRainbow[hueColor%7],hueBrit,255);
          Serial.printf("Hand Position: %i\n",handPos);
        handTimer.startTimer(1000);
      }
      prevHandPos = handPos;
      break;

    case 8:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        hueBrit = hueBrit-25;
        setHue(BULB,onOff,HueRainbow[hueColor%7],hueBrit,255);
        Serial.printf("Hand Position: %i\n",handPos);
        handTimer.startTimer(1000);
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
      pixelFill(0,red);
    }
    else if(measure1.RangeMilliMeter >100 && measure1.RangeMilliMeter <=200){
      handPos = 4;
      pixelFill(0,white);
    }
    else if(measure1.RangeMilliMeter >200 && measure1.RangeMilliMeter <=320){
      handPos = 1;
      pixelFill(0,green);
    }
  } else {
    handPos = 0;
    pixelFill(0,black);
  } 
if(handPos == 0 ){
  lox2.rangingTest(&measure2, false); // pass in 'true' to get debug data printout!

  if (measure2.RangeStatus != 4) {  // phase failures have incorrect data
    if(measure2.RangeMilliMeter <= 100){
      handPos = 8;
      pixelFill(1,red);
    }
    else if(measure2.RangeMilliMeter >100 && measure2.RangeMilliMeter <=200){
      handPos = 5;
      pixelFill(1,white);
    }
    else if(measure2.RangeMilliMeter >200 && measure2.RangeMilliMeter <=320){
      handPos = 2;
      pixelFill(1,green);
    }
  } else {
    handPos = 0;
    pixelFill(1,black);
  } 
}
if(handPos == 0){
  lox3.rangingTest(&measure3, false); // pass in 'true' to get debug data printout!

  if (measure3.RangeStatus != 4) {  // phase failures have incorrect data
    if(measure3.RangeMilliMeter <= 100){
      handPos = 9;
      pixelFill(2,red);
    }
    else if(measure3.RangeMilliMeter >100 && measure3.RangeMilliMeter <=200){
      handPos = 6;
      pixelFill(2,white);
    }
    else if(measure3.RangeMilliMeter >200 && measure3.RangeMilliMeter <=320){
      handPos = 3;
      pixelFill(2,green);
    }
  } else {
    handPos = 0;
    pixelFill(2,black);
  } 
}
return handPos;
}


void pixelFill(int pixelNum, int pixColor) {
  pixel.setPixelColor(pixelNum,pixColor);
  pixel.show();
}