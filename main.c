#include <stdio.h>
#include <stdint.h>
#include <nrf24.h>
#include "delay.h"

// transmitter 0 | receiver 1
#define MODE 0

uint16_t x;
uint8_t buffer[10];

void transmitter_config() {
  x = 0;
}

void transmitter_mode() {
  sprintf((char *)buffer, "envio %d", x);
  nrf24_transmitter(buffer);
  x++;
}

void receiver_mode() {
  nrf24_receiver(buffer);
  buffer[MESSAGE_SIZE]=0;
  printf("Received: %s\n",(char *)buffer);
}

void main() {
  printf("Config BLE\n");
  nrf24_config();
  printf("Running BLE\n");

  if(MODE == 0) {
    printf("BLE in transmitter mode\n");
    transmitter_config();
    while(1) {
      transmitter_mode();
    }
  }

  if(MODE == 1) {
    printf("BLE in receiver mode\n");
    while(1) {
      receiver_mode();
    }
  }
}