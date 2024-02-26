// Using a Wemos D1 Mini ie ESP8266, wired to logitech shifter, reads the contents of the EEPROM chip
// EEEPROM is an ST M95010W

/*
On Logi board, pins to DB9 connector are in this order:

Color  -  Use
Red    -  Power
White  -  MOSI
Orange -  Shifter Left / Right
Green  -  Shifter Fwd / Back
Yellow -  Select
Blue   -  Clock
Grey   -  Miso / Reverse
Black  -  Ground


if you are looking at the DB 9 cable that is on the shifter itself, pins are like this:

---------------------------------------
\  White  Orange  Yellow  Grey  Blue  /
 \                                   /
  \   Red*   Green   Red*   Black   /
   ---------------------------------
   * Red seems to be doubled up
   * perspective is if you are looking at the end of the DB9 plug
         that is on the original Logitech shifter's cable


  
Use the above to connect the appropriate pins below
3.3v works

*/

#include <SPI.h>

#define READ 0x03 //read command
#define RDSR 0x05 // read status register


// Define the pins used for SPI communication
const int SCK_PIN = 14; // GPIO14 (D5)   - Clock
const int MOSI_PIN = 13; // GPIO13 (D7)  - MOSI
const int MISO_PIN = 12; // GPIO12 (D6)  - MISO
const int CS_PIN = 15; // GPIO15 (D8)    - Select



// Define the SPI connection params
//                             Hz   , Bit Order,  Mode
const SPISettings SPI_Config(10000000, MSBFIRST, SPI_MODE0);


// Read bytes at address
uint8_t readByteAt(uint8_t cs, uint8_t adr) {
  SPI.beginTransaction(SPI_Config);
  digitalWrite(cs, LOW);
  uint8_t b=highByte(adr);
  b = b <<3;
  b = b | 0x03;

  SPI.transfer(b);
  SPI.transfer(lowByte(adr));
  b= SPI.transfer(0);
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
  return b;
}

// Read status register
uint8_t readStatus(uint8_t cs) {
  SPI.beginTransaction(SPI_Config);
  digitalWrite(cs, LOW);
  uint8_t b=0x05;
  SPI.transfer(lowByte(b));
  b= SPI.transfer(0);
  digitalWrite(cs, HIGH);
  SPI.endTransaction();
  return b;
}





void setup() {
  Serial.begin(9600);

  // Set up SPI communication
  pinMode(SCK_PIN, OUTPUT);
  pinMode(MOSI_PIN, OUTPUT);
  pinMode(MISO_PIN, INPUT);
  pinMode(CS_PIN, OUTPUT);
  digitalWrite(CS_PIN, HIGH);  // set chip select high , when it goes low, it selects

  SPI.pins(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  SPI.setClockDivider( SPI_CLOCK_DIV8 ); // slow the SPI bus down
  SPI.begin();

}








/// @brief 
void loop() {

    char ch[30];
    for (uint16_t i=0; i<128; i++) {
        if ((i&15) ==0) {
            sprintf(ch, "\n0x%03X: ", i);
            Serial.print(ch);
        }
    sprintf(ch, "0x%02X", readByteAt(CS_PIN, i));
    Serial.print(ch);
    }



    Serial.println();
    sprintf(ch, "0x%02X", readStatus(CS_PIN));
    Serial.print(ch);
    Serial.println();
    delay(5000);
}

