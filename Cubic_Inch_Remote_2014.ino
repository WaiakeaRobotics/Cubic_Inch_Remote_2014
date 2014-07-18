
// ================================================================
// ===                      Library Includes                    ===
// ================================================================

#include <SPI.h>             // Library for SPI communications used by the nRF24L01 radio
#include <RH_NRF24.h>        // Max we can send is 28 bytes of data 
RH_NRF24 nrf24(14, 15);      //CE, CSN

#include "LOCAL_EEPROMex.h"  // Library allowing for storage of more complicated(larger) variables in EEPROM non volatile (Flash) Memory

#include <Wire.h>          // Library for I2C communications used by OLED
#include <SeeedOLED.h>     // OLED Display library - this library was modified from the original to turn on the internal charge pump
#include <avr/pgmspace.h>
#include "bitmap.h"        // Contains the bitmap splash screen image for the remote - a little "bling" you may customize
SeeedOLED Oled;            // Reference the SeeedOLED library to Oled

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

#define IR_38Khz 3    // IR LED 38khz Anode connection - only used by 1 Cubic Inch robot
// IR LED cathode connected to arduino TX pin to enable transmitting serial data using IR and normal serial UART


// ================================================================
// ===                  Variable Definitions                    ===
// ================================================================

uint8_t sendBuffer[RH_NRF24_MAX_MESSAGE_LEN]; // 28 element array of unsigned 8-bit type - 28 is the max message length for the nrf24L01 radio

unsigned char buttons = 0; // holds current value of all 8 buttons using bit values

unsigned int counter;
int yaw;
int robotBattVoltage;
 // the receive variable type must be the same as the type being received

int slowTimer; //timer for screen update

// ================================================================
// ===                      INITIAL SETUP                       ===
// ================================================================

void setup(){
  
  Serial.begin(115200); // start the serial port at 115,200 baud
  
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
  
  Wire.begin();	//initialize I2C library
  Oled.init();  //initialize  OLED display
  DDRB|=0x21;         
  PORTB |= 0x21;
  Oled.clearDisplay();             // clear the screen and set start position to top left corner
  Oled.drawBitmap(bitmap,1024);   // 1024 = 128 Pixels * 64 Pixels / 8
  delay(1000); // delay 2 seconds so the splash screen is visible
  
  Oled.clearDisplay();          //clear the screen and set start position to top left corner
  Oled.setNormalDisplay();      //Set display to normal mode (i.e non-inverse mode)
  Oled.setPageMode();           //Set addressing mode to Page Mode
  Oled.setTextXY(0,0);          //Set the cursor to Xth Page, Yth Column  
  Oled.putString("Buttons: "); //Print the String 
  Oled.setTextXY(0,9);          //Set the cursor to Xth Page, Yth Column

  Oled.setTextXY(1,0);          //Set the cursor to Xth Page, Yth Column  
  Oled.putString("Yaw: "); //Print the String 

  Oled.setTextXY(2,0);          //Set the cursor to Xth Page, Yth Column  
  Oled.putString("Robot_V: "); //Print the String 
  
  Oled.setTextXY(5,0);          //Set the cursor to Xth Page, Yth Column  
  Oled.putString("Gyro_T: "); //Print the String 
  
  Oled.setTextXY(6,0);          //Set the cursor to Xth Page, Yth Column  
  Oled.putString("RX_T: "); //Print the String 
  
  Oled.setTextXY(7,0);          //Set the cursor to Xth Page, Yth Column  
  Oled.putString("Counter: "); //Print the String 
 
  
// ===================++===========================================
// ===             nrF34L01 Transceiver Setup                   ===
// ================================================================

  if (!nrf24.init()) Serial.println("init failed");
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(1)) Serial.println("setChannel failed");
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm)) Serial.println("setRF failed");    
  
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

  sendBuffer[0] = buttons; 
  nrf24.send(sendBuffer, sizeof(sendBuffer));

  nrf24.waitPacketSent();   // Now wait for a reply


  
// ================================================================
// ===                  Read Data From Robot                    ===
// ================================================================  
  if (nrf24.waitAvailableTimeout(10))
  { 
  uint8_t receiveBuffer[RH_NRF24_MAX_MESSAGE_LEN];
  uint8_t len = sizeof(receiveBuffer);
  
  
  if (nrf24.recv(receiveBuffer, &len))
  {
    yaw = receiveBuffer[0];
    robotBattVoltage = receiveBuffer[1];
    Serial.print("Radio TXTime: ");
    Serial.println(receiveBuffer[2],DEC);
    /*
    Oled.setTextXY(0,9);   //Set the cursor to Xth Page, Yth Column
    Oled.putChar(buttons); //Print the String
    Oled.setTextXY(1,5);   //Set the cursor to Xth Page, Yth Column
    Oled.putNumber(yaw);   //Print the variable
    Oled.putString("  ");  //Blank space to erase previous characters
    
    Oled.setTextXY(2,9);   //Set the cursor to Xth Page, Yth Column
    Oled.putNumber(robotBattVoltage); //Print the String
    Oled.putString("   "); //Blank space to erase previous characters
    
    Oled.setTextXY(5,8);          //Set the cursor to Xth Page, Yth Column  
    Oled.putNumber(receiveBuffer[3]); //Print the String
    Oled.putString("   "); //Blank space to erase previous characters 
    
    Oled.setTextXY(6,8);          //Set the cursor to Xth Page, Yth Column  
    Oled.putNumber(receiveBuffer[2]); //Print the String
    Oled.putString("   "); //Blank space to erase previous characters 

    Oled.setTextXY(7,9);          //Set the cursor to Xth Page, Yth Column  
    Oled.putNumber(counter); //Print the String
    
   */
  } 
  else{
    Serial.println("fail");
  }
 }
  //delay(1);
// ================================================================
// ===                   Write to OLED Display                  ===
// ================================================================
  counter++;
  slowTimer++;
  if (slowTimer > 2){
    slowTimer = 0;
    //Oled.setTextXY(7,9);          //Set the cursor to Xth Page, Yth Column  
    //Oled.putNumber(counter); //Print the String
  }

}

