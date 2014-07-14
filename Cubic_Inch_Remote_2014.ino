
// ================================================================
// ===                      Library Includes                    ===
// ================================================================

#include <SPI.h>  // Library for SPI communications used by the nRF24L01 radio
#include <Wire.h> // Library for I2C communications used by OLED
#include <OLED.h> // Library for the 128x64 pixel OLED display

OLED OLED; // Reference the OLED library to the OLED command name?

#include "LOCAL_nRF24L01p.h" // nRF24L01 library wait time modified so as not to slow down the program if signal is lost
#include "LOCAL_EEPROMex.h"  // Library allowing storing of more complicated variables in EEPROM non volatile (Flash) Memory

nRF24L01p transmitter(15,14);//CSN,CE //Setup radio as transmitter with CSN on arduino pin 15, CE on 14

// ================================================================
// ===                      Robot Pin Defines                   ===
// ================================================================


// ================================================================
// ===                  Variable Definitions                    ===
// ================================================================

String message;
 // the receive variable type must be the same as the type being received
int PRXsays; 
//String PRXsays;

int slowTimer; //timer for screen update

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup(){
// ================================================================
// ===                     Robot Pin Setup                      ===
// ================================================================

  //delay(150);
  Serial.begin(115200);
  SPI.begin();
  //SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  transmitter.channel(90);
  transmitter.RXaddress("CIRem");
  transmitter.TXaddress("CIBot");
  transmitter.init();
  Serial.println("I'm PTX as transceiver");
  
// ================================================================
// ===                  OLED Display Setup                      ===
// ================================================================ 
  
  OLED.init(0x3D);  //initialze  OLED display
  
  OLED.display(); // show splashscreen
  delay(2000);
  OLED.clearDisplay();
  
  OLED.setTextSize(1);
  OLED.setTextColor(WHITE);
  OLED.setCursor(0,0);
  OLED.println("Hello, world!");
  OLED.display();
}



// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop(){
  
  /*
  if(Serial.available()>0){
    char incomming=Serial.read();
    if(incomming=='\n'){
      transmitter.txPL(message);
      transmitter.send(SLOW);
      message="";
    }
    else{
      message+=incomming;
    }
  }
  */
  if(transmitter.available()){
    transmitter.read();
    transmitter.rxPL(PRXsays);
    Serial.print("PRX says: \"");
    Serial.print(PRXsays);
    Serial.println("\"");
    //PRXsays="";
    
    slowTimer++;
    if (slowTimer > 20){
      slowTimer = 0;
      OLED.clearDisplay();
      OLED.setCursor(0,0);
      OLED.println(PRXsays);
      OLED.display();
    }
  }
}

