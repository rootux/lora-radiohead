// rf69_reliable_datagram_client.pde
// -*- mode: C++ -*-
// Example sketch showing how to create a simple addressed, reliable messaging client
// with the RHReliableDatagram class, using the RH_RF95 driver to control a RF95 radio.
// It is designed to work with the other example rf95_reliable_datagram_server

// This is a modification of the Teensy example for RF69 trying to make it work with a RF95
// Testing on Teensy 3.5 with RFM95

#include <RHReliableDatagram.h>
#include <RH_RF95.h>
#include <SPI.h>
/************ Radio Setup ***************/

// Change to 434.0 or other frequency, must match RX's freq!
#define RF95_FREQ 915.0

#define CLIENT_ADDRESS 1
#define SERVER_ADDRESS 2

//For Teensy 3.x and T4.x the following format is required to operate correctly
//This is a limitation of the RadioHead radio drivers
#define RFM95_RST     3   // "A"
#define RFM95_CS      10  // "B"
#define RFM95_INT     digitalPinToInterrupt(2)

// Singleton instance of the radio driver
RH_RF95 driver(RFM95_CS,RFM95_INT);
//----- END TEENSY CONFIG

// Class to manage message delivery and receipt, using the driver declared above
RHReliableDatagram manager(driver, CLIENT_ADDRESS);

void setup() 
{
  Serial.begin(9600);
  
  //For Teensy 3.x and T4.x the following format is required to operate correctly
  //pinMode(LED, OUTPUT);     
  pinMode(RFM95_RST, OUTPUT);
  digitalWrite(RFM95_RST, LOW);

  Serial.println("Teensy RFM95!");
  Serial.println();

  // manual reset - added for Teensy
  digitalWrite(RFM95_RST, HIGH);
  delay(10);
  digitalWrite(RFM95_RST, LOW);
  delay(10);
  //----- END TEENSY CONFIG
bool flag = false;
  if (!manager.init()){
    Serial.println("init failed");
  } else {
    Serial.println("init OK!");
    flag = true;
  }
  while(1){
    Serial.println(flag);
    Serial.println();
    delay(100);
  }
  // Defaults after init are 434.0MHz, 13dBm, Bw = 125 kHz, Cr = 4/5, Sf = 128chips/symbol, CRC on
  
  if (!driver.setFrequency(RF95_FREQ)) {
    Serial.println("setFrequency failed");
  }
  
  // The default transmitter power is 13dBm, using PA_BOOST.
  // If you are using RFM95/96/97/98 modules which uses the PA_BOOST transmitter pin, then 
  // you can set transmitter powers from 2 to 20 dBm:
//  driver.setTxPower(20, false);
  // If you are using Modtronix inAir4 or inAir9, or any other module which uses the
  // transmitter RFO pins and not the PA_BOOST pins
  // then you can configure the power transmitter power for 0 to 15 dBm and with useRFO true. 
  // Failure to do that will result in extremely low transmit powers.
//  driver.setTxPower(14, true);

  // You can optionally require this module to wait until Channel Activity
  // Detection shows no activity on the channel before transmitting by setting
  // the CAD timeout to non-zero:
//  driver.setCADTimeout(10000);
}

uint8_t data[] = "Hello World!";
// Dont put this on the stack:
uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];

void loop()
{
  Serial.println("Sending to rf95_reliable_datagram_server");
    
  // Send a message to manager_server
  if (manager.sendtoWait(data, sizeof(data), SERVER_ADDRESS))
  {
    // Now wait for a reply from the server
    uint8_t len = sizeof(buf);
    uint8_t from;   
    if (manager.recvfromAckTimeout(buf, &len, 2000, &from))
    {
      Serial.print("got reply from : 0x");
      Serial.print(from, HEX);
      Serial.print(": ");
      Serial.println((char*)buf);
    }
    else
    {
      Serial.println("No reply, is rf95_reliable_datagram_server running?");
    }
  }
  else
    Serial.println("sendtoWait failed");
  delay(500);
}
