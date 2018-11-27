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

int voltage_adc;
uint8_t received = 0;

void setup() {
  pinMode(VOLTAGE_PIN, INPUT);
  TinyWire.begin(I2C_SLAVE_ADDR);
  TinyWire.onRequest(send_data);
}

void loop() {
  voltage_adc = analogRead(VOLTAGE_PIN); 
}

void send_data() {
  TinyWire.send(voltage_adc);
}
