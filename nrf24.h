#include <inttypes.h>
#include "spi.h"

// GENERAL INFOS
#define MESSAGE_SIZE  32
#define STATUS        7
#define CONFIG        0
#define PIN_CE        8

#define _BV(bit) (1 << (bit))

// COMMANDS
#define FLUSH_TX      0xE1
#define FLUSH_RX      0xE2
#define W_TX_PAYLOAD  0xA0
#define R_RX_PAYLOAD  0x61
#define FIFO_STATUS   0x17

#define R_REGISTER    0x00
#define W_REGISTER    0x20
#define EN_AA         0x01
#define SETUP_AW      0x03
#define RF_CH         0x05
#define SETUP_RETR    0x04
#define RF_SETUP      0x06
#define RX_ADDR_P0    0x0A
#define TX_ADDR       0x10
#define RX_PW_P0      0x11
#define EN_RXADDR     0x02
#define FEATURE       0x1D
#define DYNPD         0x1C
#define EN_RX_ADDR    0x02

#define MASK_RX_DR    6
#define MASK_TX_DS    5
#define MASK_MAX_RT   4
#define EN_CRC        3
#define CRCO          2
#define PWR_UP        1
#define PRIM_RX       0
#define ENAA_P5       5
#define ENAA_P4       4
#define ENAA_P3       3
#define ENAA_P2       2
#define ENAA_P1       1
#define ENAA_P0       0
#define ERX_P5        5
#define ERX_P4        4
#define ERX_P3        3
#define ERX_P2        2
#define ERX_P1        1
#define ERX_P0        0
#define AW            0
#define ARD           4
#define ARC           0
#define PLL_LOCK      4
#define RF_DR         3
#define RF_PWR        1
#define LNA_HCURR     0        
#define RX_DR         6
#define TX_DS         5
#define MAX_RT        4
#define RX_P_NO       1
#define TX_FULL       0
#define PLOS_CNT      4
#define ARC_CNT       0
#define TX_REUSE      6
#define FIFO_FULL     5
#define TX_EMPTY      4
#define RX_FULL       1
#define RX_EMPTY      0

void nrf24_config(void);
void nrf24_CE_HIGH(void);
void nrf24_CE_LOW(void);
void nrf24_power_DOWN(void);
void nrf24_power_UP(void);
void nrf24_clean_FIFO(void);
void nrf24_write_reg(uint8_t reg, uint8_t qtd, uint8_t vet[]);
uint8_t nrf24_read_reg(uint8_t reg, uint8_t qtd, uint8_t vet[]);
void nrf24_mode_TX(void);
void nrf24_mode_RX(void);
void nrf24_flush_TX(void);
void nrf24_flush_RX(void);
void nrf24_receiver(uint8_t buffer[]);
void nrf24_transmitter(uint8_t buffer[]);