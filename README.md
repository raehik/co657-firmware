# Project name
Intro.

## Project setup
This project is built for the Adafruit HUZZAH32 board using the Arduino
framework.

[PlatformIO](https://platformio.org/) is used for building and some
device management. The project was initialised with the following command:

    pio init --board=featheresp32 --ide=vim

## Dependencies
### Hardware
  * Adafruit HUZZAH32 (board `featheresp32` in PlatformIO)

### Arduino libraries
  * [NDEF](https://github.com/don/NDEF)
  * [PN532](https://github.com/elechouse/PN532) (collection of multiple libs)
  * [PubSubClient](https://github.com/knolleary/pubsubclient)

## Building
    pio run

To upload:

    pio run -t upload
