## Project setup
This project uses [PlatformIO](https://platformio.org/) for building and some
device management. I initialised the project with the following command:

    pio init --board=featheresp32 --ide=vim

We use the Arduino framework (though we're using an Espressif chip).

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
