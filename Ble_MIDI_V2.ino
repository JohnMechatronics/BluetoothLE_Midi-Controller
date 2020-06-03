


//#include "Wire.h"
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BLEMIDI.h"
#include "BluefruitConfig.h"

#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.7.0"

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
Adafruit_BLEMIDI blemidi(ble);

// prime dynamic values
int channel = 0;
bool isConnected = false;

//MIDI Controlle
int NoteON = 0x9;
int NoteOFF = 0x8;
int CC = 0xB0;

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}
void connected(void)
{
  isConnected = true;
  Serial.println(F(" CONNECTED!"));
}

void disconnected(void)
{
  Serial.println("disconnected");
  isConnected = false;
}

void setup() {

  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ) {
      error(F("Couldn't factory reset"));
    }
  }

  //ble.sendCommandCheckOK(F("AT+uartflow=off"));
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  /* Set BLE callbacks */
  ble.setConnectCallback(connected);
  ble.setDisconnectCallback(disconnected);

  Serial.println(F("Enable MIDI: "));
  if ( ! blemidi.begin(true) )
  {
    error(F("Could not enable MIDI"));
  }

  ble.verbose(false);


}

void loop() {
  //midi(channel, NoteON, 60, 100); //Send MIDI note ON (Channel, NoteON, Note, Velocity)
  //midi(channel, NoteOFF, 64, 0x0); //Send MIDI note Off (Channel, NoteON, Note, Velocity)
  midi(channel, CC, 64, 100); //Send MIDI CC (Channel, Controle Code, CC_Channelle from 64 , Value 0 -127)

}




// Send midi commands over BLE.
void midi(byte channel, byte command, byte arg1, byte arg2) {

  // init combined byte
  byte combined = command;

  // shift if necessary and add MIDI channel
  if (combined < 128) {
    combined <<= 4;
    combined |= channel;
  }
  blemidi.send(combined, arg1, arg2);

}
