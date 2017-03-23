#include <SPI.h>
#include "RF24.h"
#include "IPV4.h"

/****************** User Config ***************************/
/***      Set this radio as radio number 0 or 1         ***/
#define TIMEOUT_US  200000      // 200ms
bool radioNumber = 1;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);   //mosi miso
/**********************************************************/

byte addresses[][6] = {"1Node", "2Node"};

// Used to control whether this node is sending or receiving
char role = 2;      // starts in standby

inline void radioSetup(void);
inline void radioReceiveLoop(void);
inline void radioTransmitLoop(void);

inline void printConfigs(void);

inline void serialSetup(void)
inline void printSerialCommands(void);
inline void serialEventsLoop(void);

void setup()
{
    serialSetup();
    radioSetup();
}

void loop()
{
  /********************************** ROLES **************************************/
    switch(role){
    case 0:
        radioReceiveLoop();
        break;
    case 1:
        radioTransmitLoop();
        break;
    case 2:
    default:
        // Just do nothing
        break;
    }

  /****************** Change Roles via Serial Commands ***************************/
    serialEventsLoop();

}

inline void radioSetup(void)
{
     radio.begin();

    // Set the PA Level low to prevent power supply related issues since this is a
    // getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
    radio.setPALevel(RF24_PA_LOW);

    // Open a writing and reading pipe on each radio, with opposite addresses
    if(radioNumber){
        radio.openWritingPipe(addresses[1]);
        radio.openReadingPipe(1, addresses[0]);
    }else{
        radio.openWritingPipe(addresses[0]);
        radio.openReadingPipe(1, addresses[1]);
    }
    
    radio.enableAckPayload();
    radio.enableDynamicPayloads();

    // Start the radio listening for data
    radio.startListening(); 
}

inline void radioReceiveLoop(void)
{
    unsigned long got_msg;

    if (radio.available()) {
    // Variable for the received timestamp
    while (radio.available()){                                   // While there is data ready
        radio.read( &got_msg, sizeof(unsigned long) );              // Get the payload
    }

    radio.stopListening();                                        // First, stop listening so we can talk
    radio.write( &got_msg, sizeof(unsigned long) );               // Send the final one back.
    radio.startListening();                                       // Now, resume listening so we catch the next packets.
    Serial.print(F("Sent response "));
    Serial.println(got_msg);
  }
}

inline void radioTransmitLoop(void)
{
    radio.stopListening();                                    // First, stop listening so we can talk.

    unsigned char mymsg[] = {0x1, 0x2, 0x3, 0x4};

    Serial.println(F("Now sending"));

    unsigned long start_time = micros();                      // Take the time, and send it.  This will block until complete
    //if (!radio.write( &start_time, sizeof(unsigned long) )){
    //   Serial.println(F("failed"));
    //}
  
    if (!radio.write( &mymsg, sizeof(unsigned int) ))
        Serial.println(F("failed"));

    radio.startListening();                                    // Now, continue listening

    unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
    boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not

    while(!radio.available()){                            // While nothing is received
        if(micros() - started_waiting_at > TIMEOUT_US){           // If waited longer than 200ms, indicate timeout and exit while loop
            timeout = true;
            break;
        }
    }

    if(timeout){                                            // Describe the results
        Serial.println(F("Failed, response timed out."));
    }else {
        unsigned long got_time;                                 // Grab the response, compare, and send to debugging spew
        radio.read( &got_time, sizeof(unsigned long) );
        unsigned long end_time = micros();

        // Spew it
        Serial.print(F("Sent "));
        //Serial.print(start_time);
        Serial.print(mymsg[0], HEX);
        Serial.print(mymsg[1], HEX);
        Serial.print(mymsg[2], HEX);
        Serial.print(mymsg[3], HEX);
        Serial.print(F(", Got response "));
        Serial.print(got_time);
        Serial.print(F(", Round-trip delay "));
        Serial.print(end_time - start_time);
        Serial.println(F(" microseconds"));
    }

    // Try again 1s later
    delay(1000);
}

inline void printConfigs(void)
{
    Serial.print(F("nRF24L01+ Compatible Hardware: 0x"));
    Serial.println(radio.isPVariant(), HEX);

    Serial.print(F("Data Rate: 0x"));
    Serial.println(radio.getDataRate(), HEX);

    Serial.print(F("Channel: 0x"));
    Serial.println(radio.getChannel(), HEX);

    Serial.print(F("CRC Length: 0x"));
    Serial.println(radio.getCRCLength(), HEX);

    Serial.print(F("Payload Size: 0x"));
    Serial.println(radio.getPayloadSize(), HEX);

    Serial.print(F("Dynamic Payload Size: 0x"));
    Serial.println(radio.getDynamicPayloadSize(), HEX);

    Serial.print(F("PALevel: 0x"));
    Serial.println(radio.getPALevel(), HEX);

    Serial.print(F("Available Data in FIFO buffers: 0x"));
    Serial.println(radio.available(), HEX);
}

inline void serialSetup(void)
{
    Serial.begin(115200);
    printSerialCommands(); 
}

inline void printSerialCommands(void)
{
    Serial.println(F("*** PRESS 'T' to begin transmitting to the other node"));
    Serial.println(F("*** PRESS 'R' to begin receiving from the other node"));
    Serial.println(F("*** PRESS '?' to print the configurations"));
    Serial.println(F("*** PRESS 'S' to enter standby mode"));
}

inline void serialEventsLoop(void)
{
    if ( Serial.available() ) {
    char c = toupper(Serial.read());
    if ( c == 'T' && role != 1 ) {
      Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
      role = 1;                // Become the primary transmitter (ping out)

    } else if ( c == 'R' && role != 0 ) {
      Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));
      role = 0;                // Become the primary receiver (pong back)
      radio.startListening();

    } else if (c == '?') {
      printConfigs();

    } else if (c == 'S') {
      role = 2;               // Just stands by
      Serial.println(F("Transmit/Receive OFF"));
    }
  }
 
}
 
