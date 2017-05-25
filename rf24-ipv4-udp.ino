#include <SPI.h>
#include "RF24.h"       //http://maniacbug.github.io/RF24/classRF24.html
#include "IPV4.h"
#include <stdint.h>

/********************* User Config ************************/
/***      Set this radio as radio number 0 or 1         ***/
#define TIMEOUT_US  200000      // 200ms
bool radioNumber = 0;           // 
uint32_t local_ip = 0;
uint32_t remote_ip = 0;
uint32_t broadcast_ip = 0;

/* Hardware configuration: Set up nRF24L01 radio on SPI bus plus pins 7 & 8 */
RF24 radio(7, 8);   //mosi miso

/******************* Global Variables *********************/

byte addresses[][6] = {"1Node", "2Node"};

const uint8_t num_channels = 128;
uint8_t values[num_channels];

// Used to control whether this node is sending or receiving
char role = 2;      // starts in standby

/****************** Function Prototypes *******************/

inline void radioSetup(void);
inline void radioReceiveLoop(void);
inline void radioTransmitLoop(void);
inline void radioScanLoop(void);

inline void printConfigs(void);

inline void serialSetup(void);
inline void printSerialCommands(void);
inline void serialEventsLoop(void);

/**********************************************************/

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
        radioScanLoop();
        break;
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

const int num_reps = 100;

inline void radioScanLoop(void)
{
    // Clear measurement values
    memset(values,0,sizeof(values));

    // Scan all channels num_reps times
    int rep_counter = num_reps;
    while (rep_counter--){
        int i = num_channels;
        while (i--){
            // Select this channel
            radio.setChannel(i);

            // Listen for a little
            radio.startListening();
            delayMicroseconds(128);
            radio.stopListening();

            // Did we get a carrier?
            if ( radio.testCarrier() ) ++values[i];
        }
    }

    // Print out channel measurements, clamped to a single hex digit
    int i = 0;
    while ( i < num_channels ){
        printf("%x",min(0xf,values[i]&0xf));
        ++i;
    }
    printf("\n\r");
}

inline void radioReceiveLoop(void)
{
    // cria variaveis locais para enviar o pacote
    struct          iphdr mheader;
    struct          ippkg mpkg;

    // cria uma variável local do tamanho indicado pelo cabeçalho, para receber os dados
    uint8_t *tmp = (uint8_t *) malloc(4*sizeof(uint8_t));

    // associa o cabeçalho com o pacote (na função de transmissão isto é feito dentro da função makePackage() )
    mpkg.header = &mheader;

    // se tiver dados no buffer de recebimento
    if (radio.available()) {

        // espera encher o buffer para ter pelo menos o tamanho do cabeçalho
        while (radio.available() < 20);

        // passa os bytes do buffer de recebimento (20 bytes) para o cabeçalho alocado (mheader)
        radio.read(&mheader, sizeof(mheader));

        // conversão para o ponteiro que a função pede (no caso, int* ao invés de struct mheader )
        int *headertmp = (int *) &mheader; 

        bool err = false;

        // verifica o checksum
        err = !check_csum(headertmp, sizeof(mheader), 10);

        // verifica se a mensagem é para ele
        err = !(mheader.daddr != local_ip || (mheader.daddr != broadcast_ip));

        // espera encher o buffer para ter pelo menos o tamanho indicado no cabeçalho
        while (radio.available() < (mheader.len -20));

        // associa os dados com o pacote (na função de transmissão isto é feito dentro da função makePackage() )
        mpkg.data = tmp;

        radio.read(tmp, mheader.len -20);
    }

//////////////////////////// TODO: MOSTRAR MENSAGEM PARA A CAMADA DE APLICAÇÃO

        // radio.read(&tmp, mheader.len);

        // Serial.print("got Message: ");
        // Serial.println(got_msg, HEX);

        // radio.stopListening();                                        // First, stop listening so we can talk
        // radio.write( &got_msg, sizeof(got_msg) );               // Send the final one back.
        // radio.startListening();                                       // Now, resume listening so we catch the next packets.
        // Serial.print(F("Sent response "));
        // Serial.println(got_msg);
    //}
}

inline void radioTransmitLoop(void)
{
    radio.stopListening();                                    // First, stop listening so we can talk.

    struct          iphdr mheader;
    struct          ippkg mpkg;

    mkHeader(&mheader, local_ip, remote_ip);

    uint8_t mymsg[] = {0x1, 0x2, 0x3, 0x4};

    makePackage(&mheader, mymsg, sizeof(mymsg), &mpkg);

    Serial.println(F("Now sending"));

    uint32_t start_time = micros();                      // Take the time, and send it.  This will block until complete
    //if (!radio.write( &start_time, sizeof(uint32_t) )){
    //   Serial.println(F("failed"));
    //}
  
    if (!radio.write(&mpkg, mheader.len))         // try to send
        Serial.println(F("failed"));

    radio.startListening();                              // Now, continue listening

    uint32_t started_waiting_at = micros();              // Set up a timeout period, get the current microseconds
    boolean timeout = false;                             // Set up a variable to indicate if a response was received or not

    while(!radio.available()){                           // While nothing is received
        if(micros() - started_waiting_at > TIMEOUT_US){           // If waited longer than 200ms, indicate timeout and exit while loop
            timeout = true;
            break;
        }
    }

    if(timeout){                                            // Describe the results
        Serial.println(F("Failed, response timed out."));
    }else {
        uint32_t end_time = micros();

        // Spew it
        Serial.print(F("Sent "));
        //Serial.print(start_time);
        Serial.print(mpkg.data[0], HEX);
        Serial.print(mpkg.data[1], HEX);
        Serial.print(mpkg.data[2], HEX);
        Serial.print(mpkg.data[3], HEX);
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
    Serial.println(F("*** SEND 'T' to begin transmitting to the other node"));
    Serial.println(F("*** SEND 'R' to begin receiving from the other node"));
    Serial.println(F("*** SEND 'S' to enter scan mode"));
    Serial.println(F("*** SEND 'D' to enter Destination IP"));
    Serial.println(F("*** SEND '?' to print the configurations"));
    Serial.println(F("*** SEND '0' to enter standby mode"));
}

inline void serialEventsLoop(void)
{
    if ( Serial.available() ) {
    char c = toupper(Serial.read());
    if ( ((c == 'T') || (c == 't')) && (role != 1) ) {
        Serial.println(F("*** CHANGING TO TRANSMIT ROLE -- PRESS 'R' TO SWITCH BACK"));
        role = 1;                // Become the primary transmitter
        radio.stopListening();

    } else if ( ((c == 'R') || (c == 'r')) && (role != 0) ) {
        Serial.println(F("*** CHANGING TO RECEIVE ROLE -- PRESS 'T' TO SWITCH BACK"));
        role = 0;                // Become the primary receiver
        radio.startListening();

    } else if (c == '?') {
        printConfigs();

    } else if (c == '0') {
        role = 2;               // Just stands by
        radio.stopListening();
        Serial.println(F("Transmit/Receive OFF"));

    } else if ((c == 'S') || (c == 's')) {
        Serial.println(F("*** CHANGING TO SCAN"));

        // Print out header, high then low digit
        int i = 0;
        while ( i < num_channels ){
            printf("%x",i>>4);
            ++i;
        }
        Serial.println("");
        i = 0;
        while ( i < num_channels ){
            printf("%x",i&0xf);
            ++i;
        }
        Serial.println("");
    } else if ( ((c == 'D') || (c == 'd')) ) {
        Serial.println(F("*** PLEASE SEND THE DESTINATION IP (xxx.xxx.xxx.xxx): "));
        
        uint32_t started_waiting_at = millis();
        bool timeout = false;

        while(Serial.available() < 15){
            if(millis() - started_waiting_at > 10000){
                timeout = true;
                break;
            }
        } 
        if(timeout) Serial.println("...ops, timeout!");
        else {
            char ip[15];
            // read as a char array
            for(int i = 0; i < 15; i++) ip[i] = Serial.read();
            ip[15] = '\0'; // then terminate the string with null byte

            remote_ip = inet_addr(ip);
        }

    } else if ( ((c == 'L') || (c == 'l')) ) {
        Serial.println(F("*** PLEASE SEND THE LOCAL IP (xxx.xxx.xxx.xxx): "));
        
        uint32_t started_waiting_at = millis();
        bool timeout = false;

        while(Serial.available() < 15){
            if(millis() - started_waiting_at > 10000){
                timeout = true;
                break;
            }
        } 
        if(timeout) Serial.println("...ops, timeout!");
        else {
            char ip[15];
            // read as a char array
            for(int i = 0; i < 15; i++) ip[i] = Serial.read();
            ip[15] = '\0'; // then terminate the string with null byte

            local_ip = inet_addr(ip);
            broadcast_ip = local_ip | 255;
            Serial.print("got");
            Serial.print(ip);
            Serial.print(" (");
            Serial.print(local_ip, DEC);
            Serial.println(")");
        }

    } 
  } 
}
