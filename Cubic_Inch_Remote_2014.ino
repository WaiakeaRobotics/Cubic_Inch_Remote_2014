
// ================================================================
// ===                      Library Includes                    ===
// ================================================================

#include <SPI.h>  // Library for SPI communications used by the nRF24L01 radio
#include <RH_NRF24.h> // Max we can send is 28 bytes of data 
RH_NRF24 nrf24(14, 15); //CSN,CE //Setup radio as radio with CSN on arduino pin 15, CE on 14

#include "LOCAL_EEPROMex.h"  // Library allowing storing of more complicated variables in EEPROM non volatile (Flash) Memory

#include <Wire.h> // Library for I2C communications used by OLED
#include <SeeedOLED.h> // OLED Display library
#include <avr/pgmspace.h>
#include "bitmap.h"    // Contains the bitmap splash screen image for the remote - a little "bling" you may customize
SeeedOLED Oled; // Reference the SeeedOLED library to Oled

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

uint8_t data[1];

unsigned char buttons = 0; // holds current value of all 8 buttons using bit values
unsigned char buttonsTemp = 0; // holds previous button values to check if something changed
boolean waiting=false;
int transmitCounter;

String message;
int yaw;
int robotBattVoltage;
 // the receive variable type must be the same as the type being received
int PRXsays; 
//String PRXsays;

int slowTimer; //timer for screen update

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup(){
  
  Serial.begin(115200);
  
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

  

  
// ================================================================
// ===                  OLED Display Setup                      ===
// ================================================================ 
 
    
    /*
  Wire.begin();	
  Oled.init();  //initialze  OLED display
  DDRB|=0x21;         
  PORTB |= 0x21;

  Oled.clearDisplay();               // clear the screen and set start position to top left corner
  Oled.drawBitmap(bitmap,1024);   // 1024 = 128 Pixels * 64 Pixels / 8
  
  Oled.clearDisplay();          //clear the screen and set start position to top left corner
  Oled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  Oled.setPageMode();           //Set addressing mode to Page Mode
  Oled.setTextXY(0,0);          //Set the cursor to Xth Page, Yth Column  
  Oled.putString("Hello World!"); //Print the String
  
  
  Oled.setTextXY(0,0);          //Set the cursor to Xth Page, Yth Column  
  Oled.putString("Hello World!"); //Print the String
  Oled.setTextXY(1,0);          //Set the cursor to Xth Page, Yth Column 
  Oled.putString("Waiakea Robotics"); //Print the String
  Oled.setTextXY(2,0);          //Set the cursor to Xth Page, Yth Column
  Oled.putString("Yaw: "); //Print the String 
  Oled.setTextXY(2,6);          //Set the cursor to Xth Page, Yth Colum
  Serial.print("Buttons: ");
  
  */
  
  Wire.begin();	
  Oled.init();  //initialze  OLED display
  DDRB|=0x21;         
  PORTB |= 0x21;
  Oled.clearDisplay();             // clear the screen and set start position to top left corner
  Oled.drawBitmap(bitmap,1024);   // 1024 = 128 Pixels * 64 Pixels / 8
  delay(2000); // delay 2 seconds so the splash screen is visible
  
  Oled.clearDisplay();          //clear the screen and set start position to top left corner
  Oled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  Oled.setPageMode();           //Set addressing mode to Page Mode
  Oled.setTextXY(0,0);          //Set the cursor to Xth Page, Yth Column  
  Oled.putString("Buttons: "); //Print the String 
  Oled.setTextXY(0,9);          //Set the cursor to Xth Page, Yth Column
  //Oled.putNumber(buttons); //Print the String
  Oled.setTextXY(1,0);          //Set the cursor to Xth Page, Yth Column  
  Oled.putString("Yaw: "); //Print the String 
  //Oled.setTextXY(1,5);          //Set the cursor to Xth Page, Yth Column
  //Oled.putNumber(yaw); //Print the String
  Oled.setTextXY(2,0);          //Set the cursor to Xth Page, Yth Column  
  Oled.putString("Robot_V: "); //Print the String 
 
  
// ===================++===========================================
// ===             nrF34L01 Transceiver Setup                   ===
// ================================================================

  if (!nrf24.init())
    Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1))
    Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm))
    Serial.println("setRF failed");    
  
  Serial.println("Hi I'm your Remote");
  
  

  
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
  
    //radio.txPL(buttons);
    //radio.send(FAST);
    //Serial.println(buttons,BIN);
    
    
  data[0] = buttons;
  nrf24.send(data, sizeof(data));
  
  nrf24.waitPacketSent();
  // Now wait for a reply
  uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(buf);


    if (nrf24.recv(buf, &len))
    {
      yaw = buf[0];
      robotBattVoltage = buf[1];
      Serial.print("Yaw: ");
      Serial.println(yaw,DEC);
    }
    else
    {
      //Serial.println("recv failed");
    }

  //delay(400);
  //Serial.println("loop running");

// ================================================================
// ===                   Write to OLED Display                  ===
// ================================================================

  slowTimer++;
  if (slowTimer > 5){
    slowTimer = 0;
    
    //radio.txPL(buttons);
    //radio.send(SLOW);
    
    Oled.setTextXY(0,9);          //Set the cursor to Xth Page, Yth Column
    Oled.putChar(buttons); //Print the String
    Oled.setTextXY(1,5);          //Set the cursor to Xth Page, Yth Column
    Oled.putNumber(yaw); //Print the String
    Oled.putString("  "); //Print the String
    Oled.setTextXY(2,9);          //Set the cursor to Xth Page, Yth Column
    Oled.putNumber(robotBattVoltage); //Print the String
    Oled.putString("   "); //Print the String
    
    Serial.print("Buttons: ");
    Serial.println(buttons,BIN);
  }

}

