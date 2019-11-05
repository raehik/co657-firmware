# Hardware
  * Adafruit HUZZAH32 (ESP32)
    * aka Adafruit ESP32 Feather
    * Also referred to as just **ESP32**.
  * Elechouse PN532 NFC RFID Module V3
    * Also referred to as just **PN532**.

## ESP32
[Booking page](https://booking.cs.kent.ac.uk/item/40)

## PN532
  * [Booking page](https://booking.cs.kent.ac.uk/accessory/45)
  * [Product page](https://www.elechouse.com/elechouse/index.php?main_page=product_info&cPath=90_93&products_id=2242)

Seems to automatically switch between 3.3V and 5V for you - thank goodness,
because the ESP32 only provides 3.3V.

From the booking page, supported NFC technologies:

  * Mifare 1k (what Kent One cards use), 4k, Ultralight, and DesFire cards
  * ISO/IEC 14443-4 cards such as CD97BX, CD light, Desfire, P5CN072 (SMX)
  * Innovision Jewel cards such as IRT5001 card
  * FeliCa cards such as RCS_860 and RCS_854

### Libraries
#### Arduino
  * [NDEF: Common NFC message library for Arduino](https://github.com/don/NDEF)
  * [Elechouse's extended PN532 library](https://github.com/elechouse/PN532)
