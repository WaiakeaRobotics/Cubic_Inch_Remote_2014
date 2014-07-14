#include <SPI.h>
#include "LOCAL_nRF24L01p.h"
#include "LOCAL_EEPROMex.h"

nRF24L01p transmitter(14,8);//CSN,CE

void setup(){
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
}

String message;

 // the receive variable type must be the same as the type being received
int PRXsays; 
//String PRXsays;

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
  }
}

