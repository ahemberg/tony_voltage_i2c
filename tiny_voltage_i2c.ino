#include <TinyWire.h>
#include <twi.h>

/*
 * Simple sketch that configures an attiny85 as I2c Slave. The tiny measures
 * voltage on one of its pins continously and sends it off to the I2c Master
 * when requested.
 *
 * Uses TinyWire.h
 *
 */

#define I2C_SLAVE_ADDR  0x26
#define VOLTAGE_PIN A2

union Uint16AsBytes {
  uint16_t ival;
  uint8_t bval[2];
};

uint16_t voltage_adc;
bool sending = false;
uint8_t byte_counter = 0;

Uint16AsBytes voltage_un;

void setup() {
  pinMode(VOLTAGE_PIN, INPUT);

  TinyWire.begin(I2C_SLAVE_ADDR);
  //TinyWire.onReceive(set_mode); TODO: Implement maybe? Request different ADCs
  TinyWire.onRequest(send_data);
}

void loop() {
  if (!sending) {
    voltage_adc = analogRead(VOLTAGE_PIN);
    voltage_un.ival = voltage_adc;
  }
}

void send_data() {
    bool sending = true;
    TinyWire.send(voltage_un.bval[byte_counter]);
    byte_counter++;

    if (byte_counter >= 2) {
      byte_counter = 0;
      sending = false;
    }
}
