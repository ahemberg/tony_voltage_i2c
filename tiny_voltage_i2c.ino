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

class DataPacket {
public:
  bool sending = false;
  void set_data(uint16_t data_u) {
    num_bits = sizeof(data_u);
    Uint16AsBytes datau;
    datau.ival = data_u;

    for (uint8_t i = 0; i<num_bits; i++) {
      data[i] = datau.bval[i];
    }

    calculate_checksum();
  }

  void send_byte() {
    uint8_t out_byte = 0x00;
    if (pointer < 3) {
      out_byte = preamble[pointer];
    } else if (pointer == 3) {
      out_byte = num_bits;
    } else if (pointer > 3 && pointer <= 3+num_bits) {
      out_byte = data[pointer-4];
    } else if (pointer == 4+num_bits) {
      out_byte = checksum;
    }

    TinyWire.send(out_byte);
    pointer++;

    if (pointer > num_bits + 4) {
      pointer = 0;
      sending = false;
    } else {
      sending = true;
    }
  };
private:

  void calculate_checksum() {

    uint16_t checksum_l = 0;

    for (uint8_t i = 0; i< num_bits; i++) {
      checksum_l += data[i];
    }

    checksum = checksum_l % 0xfd;
  }

  uint8_t num_bits { 0x00 };
  uint8_t checksum { 0x00 };
  uint8_t pointer { 0x00 };
  uint8_t preamble[3] {
    0xfd,
    0xfc,
    0xfd
  };
  uint8_t data[128];

};

uint16_t voltage_adc;
bool sending = false;
uint8_t byte_counter = 0;

DataPacket packet;

void setup() {
  pinMode(VOLTAGE_PIN, INPUT);

  TinyWire.begin(I2C_SLAVE_ADDR);
  //TinyWire.onReceive(set_mode); TODO: Implement maybe? Request different ADCs
  TinyWire.onRequest(send_data);
}

void loop() {
  if (!packet.sending) {
    voltage_adc = analogRead(VOLTAGE_PIN);
    packet.set_data(voltage_adc);
  }
}

void send_data() {
    packet.send_byte();
}
