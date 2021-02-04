/**************************************************************************/
/*! 

This example is for communicating with the PN532 chip using I2C. Wiring 
should be as follows:
  PN532 SDA -> EFM32 D22 Pin (with 4.7K resistor already present)
  PN532 SCL -> EFM32 D21 Pin(with 4.7K resistor already present)
  PN532 IRQ -> EFM32 D4 Pin
  PN32 RST0 -> EFM32 D5 Pin
  PN532 3.3v -> 3.3v
  PN532 GND -> GND

Based on readMifareClassicIrq.pde by Adafruit
*/
/**************************************************************************/
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

static void startListeningToNFC();
static void handleCardDetected();

// Pins used for I2C IRQ
#define PN532_IRQ   4
#define PN532_RESET 5 

const int DELAY_BETWEEN_CARDS = 500;
long timeLastCardRead = 0;
boolean readerDisabled = false;
int irqCurr;
int irqPrev;

// This example uses the IRQ line, which is available when in I2C mode.
Adafruit_PN532 nfc(PN532_IRQ, PN532_RESET);

void setup(void) {
  Serial.begin(115200); //Adapt the platformio.ini with correct monitor_speed

  Serial.println("Begin NFC532 Scanning Software.");

  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN532 board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5"); Serial.println((versiondata>>24) & 0xFF, HEX); 
  Serial.print("Firmware ver. "); Serial.print((versiondata>>16) & 0xFF, DEC); 
  Serial.print('.'); Serial.println((versiondata>>8) & 0xFF, DEC);
  
  // configure board to read RFID tags
  nfc.SAMConfig();

  startListeningToNFC();
}

void loop(void) {
  if (readerDisabled) {
    if (millis() - timeLastCardRead > DELAY_BETWEEN_CARDS) {
      readerDisabled = false;
      startListeningToNFC();
    }
  } else {
    irqCurr = digitalRead(PN532_IRQ);

    // When the IRQ is pulled low - the reader has got something for us.
    if (irqCurr == LOW && irqPrev == HIGH) {
       //Serial.println("Got NFC IRQ");  
       handleCardDetected(); 
    }
  
    irqPrev = irqCurr;
  }
}

void startListeningToNFC() {
  // Reset our IRQ indicators
  irqPrev = irqCurr = HIGH;
  
  Serial.println("Present an ISO14443A Card ...");
  nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);
}

void handleCardDetected() {
    uint8_t success = false;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

    // read the NFC tag's info
    success = nfc.readDetectedPassiveTargetID(uid, &uidLength);
    Serial.println(success ? "Read successful" : "Read failed (not a card?)");

    if (success) {
      // Display some basic information about the card
      //Serial.println("Found an ISO14443A card");
      //Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
      //Serial.print("  UID Value: ");
      Serial.print("Card ID HEX Value: ");
      nfc.PrintHex(uid, uidLength);
      
      if (uidLength == 4)
      {
        // We probably have a Mifare Classic card ... 
        uint32_t cardid = uid[0];
        cardid <<= 8;
        cardid |= uid[1];
        cardid <<= 8;
        cardid |= uid[2];  
        cardid <<= 8;
        cardid |= uid[3]; 
        //Serial.print("Seems to be a Mifare Classic card #");
        Serial.print("Card ID NUMERIC Value: ");
        Serial.println(cardid);
      }
      Serial.println("");

      timeLastCardRead = millis();
    }

    // The reader will be enabled again after DELAY_BETWEEN_CARDS ms will pass.
    readerDisabled = true;
}