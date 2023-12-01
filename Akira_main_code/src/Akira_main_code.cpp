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
#include "Adafruit_SSD1306.h"
#include "Adafruit_BME280.h"
#include "IoTTimer.h"
#include "Colors.h"
#include <neopixel.h>
#include "hue.h"
#include "Button.h"
SYSTEM_MODE(MANUAL);

Button Button(D19);

const int BULB=4; 
String colorName[10] = {"Red   ","Orange","Yellow","Green ","Blue  ","Indigo","Violet"};

const int PIXELCOUNT = 4;
Adafruit_NeoPixel pixel(PIXELCOUNT,SPI1,WS2812B);
void pixelFill(int pixelNum, int pixColor);

const int OLED_RESET = -1;
Adafruit_SSD1306 display(OLED_RESET);
Adafruit_BME280 bme;
int status;


DFRobotDFPlayerMini myDFPlayer;
Adafruit_VL53L0X lox1 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox2 = Adafruit_VL53L0X();
Adafruit_VL53L0X lox3 = Adafruit_VL53L0X();

int wheresHand();
void setID();
void musicMode(int handPos);
void lightMode(int handPos);
void tempMode(int handPos);

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
int max1 = 150,max2 = 300,max3 =450;



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
    delay(10000);
    while(true);
  }
  Serial.println(F("DFPlayer Mini online."));
  
  myDFPlayer.randomAll();
  myDFPlayer.pause();
  pausePlay = 0;
  myDFPlayer.volume(15); 


  pixel.begin();
  pixel.setBrightness(30);
  pixel.show();

  curMode = 0;

  status = bme.begin(0x76);
  if(status == false){
    Serial.printf("BME280 at address 0x%02X failed to start", 0x76);
  }
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(12,28);
  display.clearDisplay();
  display.printf("A.K.I.R.A");
  display.display();
  display.setTextSize(1);
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
      tempMode(handLoc);
    }
      break;
  }
} 
void musicMode(int handPos){
 static int curVol; //,curState;
  static int prevHandPos;
  Serial.printf("Hand Position:%i\n",handPos);
  switch(handPos){
    case 0:
        display.clearDisplay();      
        handTimer.startTimer(500);
        display.setCursor(39,28);
        display.printf("Volume:%i",curVol);
        display.display();
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
        //Serial.printf("Hand Position:%i\n",handPos);
        display.clearDisplay();
        myDFPlayer.volumeUp();
        handTimer.startTimer(500);
        curVol = myDFPlayer.readVolume();
        display.setCursor(39,28);
        display.printf("Volume:%i",curVol);
        display.display();
      }
      prevHandPos = handPos;
      break;

    case 4:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        display.clearDisplay();
        //Serial.printf("Hand Position:%i\n",handPos);
        myDFPlayer.previous();
        display.setCursor(29,28);
        display.printf("Previuos song");
        display.display();
        handTimer.startTimer(2000);
      }
      prevHandPos = handPos;
      break;

    case 5:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        handTimer.startTimer(3000);
        //Serial.printf("Hand Position:%i\n",handPos);
        display.clearDisplay();
        pausePlay = !pausePlay;
        if (pausePlay ==1 && myDFPlayer.readState() == 514){
          myDFPlayer.start(); 
          display.setCursor(50,28);
          display.printf("Play"); 
          display.display();
        }
        else if(pausePlay == 0 && myDFPlayer.readState() == 513){
          myDFPlayer.pause();
          display.setCursor(48,28);
          display.printf("Paused");
          display.display();
        }
      }
      prevHandPos = handPos;
      break;

    case 6:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        display.clearDisplay();
        //Serial.printf("Hand Position:%i\n",handPos);
        myDFPlayer.next();
        display.setCursor(39,28);
        display.printf("Next song");
        display.display();
        handTimer.startTimer(2000);
      }
      prevHandPos = handPos;
      break;

    case 8:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        //Serial.printf("Hand Position:%i\n",handPos);
        display.clearDisplay();
        myDFPlayer.volumeDown();
        handTimer.startTimer(500);
        curVol = myDFPlayer.readVolume();
        display.setCursor(39,28);
        display.printf("Volume:%i",curVol);
        display.display();
      }
      prevHandPos = handPos;
      
      break;
  }
}


void lightMode(int handPos){
  static int prevHandPos, hueBrit=125, hueColor;
  static bool onOff = 0;
  Serial.printf("Hand Position:%i\n",handPos);
  switch(handPos){
    case 0:
      display.clearDisplay();
      handTimer.startTimer(500);
      display.setCursor(28,20);
      display.printf("Brightness:%i",hueBrit);
      display.setCursor(45,30);
      display.printf("%s",colorName[hueColor%7].c_str());
      display.display();
      prevHandPos =0;
      break;

    case 2:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        display.clearDisplay();
        hueBrit = hueBrit+25;
        if (hueBrit > 255 ){
          hueBrit = 255;
        }
        setHue(BULB,onOff,HueRainbow[hueColor%7],hueBrit,255);
        //Serial.printf("Hand Position: %i\n",handPos);
        handTimer.startTimer(500);
        display.setCursor(28,28);
        display.printf("Brightness:%i",hueBrit);
        display.display();
      }
      prevHandPos = handPos;
      break;

    case 4:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        display.clearDisplay();
        if(hueColor>0){
          hueColor = hueColor-1;
        }
        setHue(BULB,onOff,HueRainbow[hueColor%7],hueBrit,255);
        display.setCursor(45,28);
        display.printf("%s",colorName[hueColor%7].c_str());
        display.display();
        handTimer.startTimer(1000);
      }
      prevHandPos = handPos;
      break;

    case 5:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        display.clearDisplay();
        onOff = !onOff;  
        setHue(BULB,onOff,HueRainbow[hueColor%7],hueBrit,255);
        //Serial.printf("Hand Position: %i\n",handPos);
        if (onOff ==1){
          display.setCursor(35,28);
          display.printf("Light on");
          display.display();
        }
        else{
          display.setCursor(35,28);
          display.printf("Light off");
          display.display();
        }
        handTimer.startTimer(1000);
      }
      prevHandPos = handPos;
      break;

    case 6:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        display.clearDisplay();
        hueColor = hueColor+1;
        setHue(BULB,onOff,HueRainbow[hueColor%7],hueBrit,255);
        display.setCursor(45,28);
        display.printf("%s",colorName[hueColor%7].c_str());
        display.display();
        handTimer.startTimer(1000);
      }
      prevHandPos = handPos;
      break;

    case 8:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        display.clearDisplay();
        hueBrit = hueBrit-25;
        if (hueBrit < 0 ){
          hueBrit = 0;
        }
        setHue(BULB,onOff,HueRainbow[hueColor%7],hueBrit,255);
        //Serial.printf("Hand Position: %i\n",handPos);
        handTimer.startTimer(500);
        display.setCursor(28,28);
        display.printf("Brightness:%i",hueBrit);
        display.display();
      }
      prevHandPos = handPos;
      break;
  }
}


void tempMode(int handPos){
  static int prevHandPos, desiredTemp = 68;
  int tempC, tempF;
  Serial.printf("Hand Position:%i\n",handPos);
  tempC = bme.readTemperature();
  tempF = map(tempC,0,85,32,185);
  switch(handPos){
    case 0:
        display.clearDisplay();
        handTimer.startTimer(500);
        display.setCursor(28,20);
        display.printf("Curent Temp:%i",tempF);
        display.setCursor(28,30);
        display.printf("Desired Temp:%i",desiredTemp);
        display.display();
      prevHandPos =0;
      break;

    case 2:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        display.clearDisplay();
        desiredTemp = desiredTemp +1;
        if (desiredTemp > 80 ){
          desiredTemp = 80;
        }
        handTimer.startTimer(500);
        display.setCursor(28,20);
        display.printf("Curent Temp:%i",tempF);
        display.setCursor(28,30);
        display.printf("Desired Temp:%i",desiredTemp);
        display.display();
      }
      prevHandPos = handPos;
      break;

    case 5:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        handTimer.startTimer(1000);
      }
      prevHandPos = handPos;
      break;

    case 8:
      if(handPos != prevHandPos){
        handTimer.startTimer(1000);
      }
      if(handTimer.isTimerReady()){
        display.clearDisplay();
        desiredTemp = desiredTemp -1;
        if (desiredTemp < 50){
          desiredTemp = 50;
        }
        handTimer.startTimer(500);
        display.setCursor(28,20);
        display.printf("Curent Temp:%i",tempF);
        display.setCursor(28,30);
        display.printf("Desired Temp:%i",desiredTemp);
        display.display();
      }
      prevHandPos = handPos;
      break;
  }
}




int wheresHand(){
  int handPos;  
 
  lox1.rangingTest(&measure1, false); // pass in 'true' to get debug data printout!

  if (measure1.RangeStatus != 4) {  // phase failures have incorrect data
    if(measure1.RangeMilliMeter >10 && measure1.RangeMilliMeter <= max1){
      handPos = 7;
      pixelFill(0,red);
    }
    else if(measure1.RangeMilliMeter >max1 && measure1.RangeMilliMeter <=max2){
      handPos = 4;
      pixelFill(0,white);
    }
    else if(measure1.RangeMilliMeter >max2 && measure1.RangeMilliMeter <=max3){
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
    if(measure2.RangeMilliMeter >10 && measure2.RangeMilliMeter <= max1){
      handPos = 8;
      pixelFill(1,red);
    }
    else if(measure2.RangeMilliMeter >max1 && measure2.RangeMilliMeter <=max2){
      handPos = 5;
      pixelFill(1,white);
    }
    else if(measure2.RangeMilliMeter >max2 && measure2.RangeMilliMeter <=max3){
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
    if(measure3.RangeMilliMeter >10 && measure3.RangeMilliMeter <= max1){
      handPos = 9;
      pixelFill(2,red);
    }
    else if(measure3.RangeMilliMeter >max1 && measure3.RangeMilliMeter <=max2){
      handPos = 6;
      pixelFill(2,white);
    }
    else if(measure3.RangeMilliMeter >max2 && measure3.RangeMilliMeter <=max3){
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