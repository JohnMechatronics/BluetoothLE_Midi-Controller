/*
BluetoothLE_Midi-Controller

This code allows a Feather 32u4 Bluefruit LE to be set up as a bluetooth midi device and 
send Midi data (Midi Note On, Off and Controlle Codes).

Requires a Feather Bluefruit LE 32u4: https://www.adafruit.com/products/2829
Required dependencies: Adafruit Bluefruit Library: https://github.com/adafruit/Adafruit_BluefruitLE_nRF51

Instructions for setting up a Feather 32u4 Bluefruit LE as a bluetooth midi device see - https://learn.adafruit.com/bluetooth-le-midi-controller/usage

Based on code originally wrote by 
Todd Treece <todd@uniontownlabs.org> See: - https://learn.adafruit.com/capacitive-touch-drum-machine/

Adapted for simple midi data by John.wild@rca.ac.uk
*/


#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BLEMIDI.h"
#include "BluefruitConfig.h"

#define FACTORYRESET_ENABLE         1
#define MINIMUM_FIRMWARE_VERSION    "0.7.0"

//MIDI Controlle
#define NoteON 0x9
#define NoteOFF 0x8
#define midiCC 0xB0

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);
Adafruit_BLEMIDI blemidi(ble);

// prime dynamic values
int channel = 0;
bool isConnected = false;

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
  
  //Send MIDI note ON (Channel | NoteON | Note |Velocity)
  midi(channel, NoteON, 60, 100); 
  //Send MIDI note Off (Channel | NoteOFF | Note | Velocity)
  midi(channel, NoteOFF, 60, 0x0); 
  //Send MIDI CC (Channel | Controle Code | CC_Channelle from 64 | Value 0-127)
  midi(channel, midiCC, 64, 100); 
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
