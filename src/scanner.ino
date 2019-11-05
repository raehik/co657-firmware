#include <NfcTag.h>
#include <NfcAdapter.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <Wire.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);

void scanner_setup(void) {
    nfc.begin();
}

void scanner_scan(void) {
    if (nfc.tagPresent()) {
        NfcTag tag = nfc.read();
        tag.print();
    }
    /*
    boolean success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;                       // Length of the UID (4 or 7 bytes depending on ISO14443A card type)

    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
    if (success) {
        Serial.println("Found A Card!");
        Serial.print("UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
        Serial.print("UID Value: ");
        for (uint8_t i=0; i < uidLength; i++) {
            Serial.print(" 0x");Serial.print(uid[i], HEX);
        }
        Serial.println("");
        // 1 second halt
        delay(1000);
    } else {
        // PN532 probably timed out waiting for a card
        Serial.println("Timed out! Waiting for a card...");
    }
    */
}
