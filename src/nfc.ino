#include <NfcTag.h>
#include <NfcAdapter.h>
#include <PN532_I2C.h>
#include <PN532.h>
#include <Wire.h>

PN532_I2C pn532_i2c(Wire);
NfcAdapter nfc = NfcAdapter(pn532_i2c);

void nfc_setup(void) {
    nfc.begin();
}

boolean nfc_tag_present(void) {
    return nfc.tagPresent();
}

void nfc_read(NfcTag* tag) {
    *tag = nfc.read();
}
