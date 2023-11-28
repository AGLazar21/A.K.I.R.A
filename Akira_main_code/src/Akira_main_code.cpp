/* 
 * Project myProject
 * Author: Your Name
 * Date: 
 * For comprehensive documentation and examples, please visit:
 * https://docs.particle.io/firmware/best-practices/firmware-template/
 */

// Include Particle Device OS APIs
#include "Particle.h"
#include <Keypad_Particle.h>
SYSTEM_MODE(SEMI_AUTOMATIC);

SYSTEM_THREAD(ENABLED);


const byte ROWS = 4;
const byte COLS = 4;
char hexaKeys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'},
};
byte colPins[COLS] = {D17,D14,D13,D12};
byte rowPins[ROWS] = {D16,D5,D17,D18};
Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS);
int handPos;
int wheresHand();


void setup() {
  Serial.begin();
  waitFor(Serial.isConnected,10000);







  /* myDFPlayer.play(1);  //Play the first mp3 */
}


void loop() {
  handPos = wheresHand();
  Serial.printf("Hand Position:%i\n",handPos);
}

int wheresHand(){
 int handPos = customKeypad.getKey();
 
 return handPos;
}

/* void musicMode(int handPos){
  switch(handPos){
    case 2:
      myDFPlayer.volumeUp();
      break;
    case 4:
      myDFPlayer.previuos();
      break;
    case 5:
      pausePlay = !pausePlay;
      timer=-9999999;
      if (onOff ==1) {
        if (millis() - timer > 60000) {
          timer = millis();
          myDFPlayer.next();  //Play next mp3 every 3 second.
        }
      }
      else{
        myDFPlayer.pause();
      }
      break;
    case 6:
      myDFPlayer.next();
      break;
    case 8:
      myDFPlayer.volumeDown();
      break;
  }
} */