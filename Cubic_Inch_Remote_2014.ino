
// ================================================================
// ===                      Library Includes                    ===
// ================================================================

#include <SPI.h>  // Library for SPI communications used by the nRF24L01 radio
#include <Wire.h> // Library for I2C communications used by OLED
#include <OLED.h> // Library for the 128x64 pixel OLED display

OLED OLED; // Reference the OLED library to the OLED command name?

#include "LOCAL_nRF24L01p.h" // nRF24L01 library wait time modified so as not to slow down the program if signal is lost
#include "LOCAL_EEPROMex.h"  // Library allowing storing of more complicated variables in EEPROM non volatile (Flash) Memory

nRF24L01p radio(15,14);//CSN,CE //Setup radio as radio with CSN on arduino pin 15, CE on 14



// ================================================================
// ===                      Robot Pin Defines                   ===
// ================================================================

#define A 2  // Right D pad up button
#define B 8  // Right D pad right button
#define C 9  // Right D pad down button
#define D 10 // Right D pad left button

#define UP 4    // Left D pad up button
#define RIGHT 7 // Left D pad right button
#define DOWN 5  // Left D pad down button
#define LEFT 6  // Left D pad left button

//#define LED 13 // Yellow indicator LED - Pin also used by OLED SCK pin so not usable during normal operation

#define BATT_VOLTAGE 18 // Battery voltage monitor pin - connected to 50% divider to allow the measurment of voltages higher than the vcc of 3.3v

#define IR_38Khz 3    // IR LED 38khz Anode connection
// IR LED cathode connected to arduino TX pin to enable transmitting serial data using IR and normal serial UART


// ================================================================
// ===                  Variable Definitions                    ===
// ================================================================

unsigned char buttons = 0; // holds current value of all 8 buttons using bit values
unsigned char buttonsTemp = 0; // holds previous button values to check if something changed
boolean waiting=false;
int transmitCounter;

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

  pinMode(A, INPUT); // Set pushbutton pins to inputs
  pinMode(B, INPUT);
  pinMode(C, INPUT);
  pinMode(D, INPUT);
  
  pinMode(UP, INPUT);
  pinMode(RIGHT, INPUT);
  pinMode(DOWN, INPUT);
  pinMode(LEFT, INPUT);
  
  pinMode(BATT_VOLTAGE,INPUT);
  
  digitalWrite(A, HIGH); // Enable pullups on push buttons
  digitalWrite(B, HIGH); // Enable pullups on push buttons
  digitalWrite(C, HIGH); // Enable pullups on push buttons
  digitalWrite(D, HIGH); // Enable pullups on push buttons
  
  digitalWrite(UP, HIGH); // Enable pullups on push buttons
  digitalWrite(RIGHT, HIGH); // Enable pullups on push buttons
  digitalWrite(DOWN, HIGH); // Enable pullups on push buttons
  digitalWrite(LEFT, HIGH); // Enable pullups on push buttons

  Serial.begin(115200);

  
// ================================================================
// ===                  OLED Display Setup                      ===
// ================================================================ 
  
  OLED.init(0x3D);  //initialze  OLED display
  OLED.display(); // show splashscreen
  delay(1000);
  
// ===================++===========================================
// ===             nrF34L01 Transceiver Setup                   ===
// ================================================================

  SPI.begin();
  //SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);
  radio.channel(0);
  radio.RXaddress("CIRem");
  radio.TXaddress("CIBot");
  radio.init();
  delay(1000);
  Serial.println("Hi I'm your Remote");
  
  
  OLED.clearDisplay();  
  OLED.setTextSize(1);
  OLED.setTextColor(WHITE);
  OLED.setCursor(0,0);
  OLED.println("Hello, world!");
  OLED.display();
  
} // End setup function

// ================================================================
// ===                    MAIN PROGRAM LOOP                     ===
// ================================================================

void loop(){
  
  
// ================================================================
// ===                       Read Buttons                       ===
// ================================================================

// We use the "bitWrite" function to only change each bit individually in the "buttons" variable depending on the state of each button.  
// This allows us to store all 8 button values in one 8 bit variable which can then be easily sent to the robot for decoding. 
  bitWrite(buttons, 0, !(digitalRead(A))); //So here the bit zero bit (right most bit) in "buttons" will be set to the current state of the button named "A"
  bitWrite(buttons, 1, !(digitalRead(B)));
  bitWrite(buttons, 2, !(digitalRead(C)));
  bitWrite(buttons, 3, !(digitalRead(D)));
  bitWrite(buttons, 4, !(digitalRead(UP)));
  bitWrite(buttons, 5, !(digitalRead(RIGHT))); 
  bitWrite(buttons, 6, !(digitalRead(DOWN)));
  bitWrite(buttons, 7, !(digitalRead(LEFT)));
  
  
// ================================================================
// ===                    Send Data to Robot                    ===
// ================================================================


    //radio.txPL(buttons);
    //radio.send(FAST);

/*

  if (buttons != buttonsTemp){ // Has the buttons variable changed since the last time loop?
    transmitCounter = 100;
  }
  buttonsTemp = buttons;
  
  if (transmitCounter >= 100){
    
    transmitCounter = 0;
    radio.txPL(buttons);
    radio.send(FAST);
  }
  transmitCounter ++;
  */
  
// ================================================================
// ===                  Read Data From Robot                    ===
// ================================================================  
  /*
  if (waiting == false){
    radio.txPL(buttons);
    radio.send(FAST);
    radio.init();
    waiting = true;
    Serial.println("transmitting");
  }
  else{
    Serial.println("waiting");
    if(radio.available()){
      waiting=false;
      radio.read();
      radio.rxPL(PRXsays);
      Serial.println(PRXsays,DEC);
      Serial.println(buttons,BIN);
      radio.init();
    }   
  }
  */
  
    radio.txPL(buttons);
    radio.send(FAST);
    //Serial.println(buttons,BIN);

// ================================================================
// ===                   Write to OLED Display                  ===
// ================================================================
/*
  slowTimer++;
  if (slowTimer > 50){
    slowTimer = 0;
    
    //radio.txPL(buttons);
    //radio.send(SLOW);
    
    OLED.clearDisplay();
    OLED.setCursor(0,0);
    OLED.println(buttons);     // Display button variable in ASCII
    OLED.println(buttons,DEC); // Display button variable in Decimal
    OLED.println(buttons,BIN); // Display button value in Binary
    OLED.println(PRXsays);     // Display the value received from the Robo
    OLED.display();
    
    Serial.println(buttons,BIN);
  }
*/
}

