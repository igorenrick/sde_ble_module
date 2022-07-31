#include "nrf24.h"

void nrf24_config(void) {
	uint8_t parametros[5];

	spi_init();

	pinMode(PIN_CE, OUTPUT);
	nrf24_CE_LOW();

	nrf24_power_DOWN();
	delay_ms(300);

	nrf24_power_UP();
	delay_ms(3000);

	nrf24_clean_FIFO();
	
	parametros[0] = 0;
    nrf24_write_reg(DYNPD, 1, parametros);
	
	parametros[0] = 1;
    nrf24_write_reg(EN_RX_ADDR, 1, parametros);
    
	parametros[0] = 0x01;
    nrf24_write_reg(EN_RXADDR, 1, parametros);

	parametros[0] = 0;
    nrf24_write_reg(FEATURE, 1, parametros);
    
	parametros[0] = 0x00;
    nrf24_write_reg(EN_AA, 1, parametros);

	parametros[0] = 0x01;
    nrf24_write_reg(SETUP_AW, 1, parametros);

	parametros[0] = 0x6E;
    nrf24_write_reg(RF_CH, 1, parametros);

	parametros[0] = 0;
	nrf24_write_reg(SETUP_RETR, 1, parametros);

	parametros[0] = 0x07;
    nrf24_write_reg(RF_SETUP, 1, parametros);

	parametros[0] = 2;
	parametros[1] = 2;
	parametros[2] = 2;
	
    nrf24_write_reg(RX_ADDR_P0, 3, parametros);

	nrf24_write_reg(TX_ADDR, 3, parametros);

	parametros[0] = MESSAGE_SIZE;
	nrf24_write_reg(RX_PW_P0, 1, parametros);

	parametros[0] = 0x0E;
	nrf24_write_reg(STATUS, 1, parametros);

	parametros[0] = 0x77;
    nrf24_write_reg(CONFIG, 1, parametros);

	parametros[0] = 0x11;
	nrf24_write_reg(FIFO_STATUS, 1, parametros);

	nrf24_clean_FIFO();
}

void nrf24_CE_HIGH(void) {
	digitalWrite(PIN_CE, HIGH);
}

void nrf24_CE_LOW(void) {
	digitalWrite(PIN_CE, LOW);
}

void nrf24_power_DOWN(void) {
	uint8_t reg_config;
	nrf24_read_reg(CONFIG, 1, &reg_config);
	reg_config = reg_config & 0xFD; 
	nrf24_write_reg(CONFIG, 1, (uint8_t *)&reg_config);
}

void nrf24_power_UP(void) {
	uint8_t reg_config;
	nrf24_read_reg(CONFIG, 1, &reg_config);
	reg_config = reg_config | 0x02; 
	nrf24_write_reg(CONFIG, 1, (uint8_t *)&reg_config);
}

void nrf24_clean_FIFO(void) {
	uint8_t origem;
	nrf24_read_reg(CONFIG, 1, &origem);

	delay_us(10);
	
	nrf24_mode_TX();
	spi_habilita();
	spi_write(FLUSH_TX);
	spi_desabilita();

    nrf24_mode_RX();
	spi_habilita();
	spi_write(FLUSH_RX);
	spi_desabilita();

	nrf24_write_reg(CONFIG, 1, (uint8_t *)&origem);
}

void nrf24_write_reg(uint8_t reg, uint8_t qtd, uint8_t vet[]) {
	uint8_t x;
	spi_habilita();
	spi_write(W_REGISTER|reg);
	for (x = 0; x < qtd; x++) {
		spi_write(vet[x]);
	}
	spi_desabilita();
}

uint8_t nrf24_read_reg(uint8_t reg, uint8_t qtd, uint8_t vet[]) {
	uint8_t x;
	spi_habilita();
	spi_write(R_REGISTER|reg);
	for (x = 0; x < qtd; x++) {
		vet[x] = spi_write(0x00);
	}
	spi_desabilita();
	return x;
}

void nrf24_mode_TX(void) {
	uint8_t reg_config;
	nrf24_read_reg(1, CONFIG, &reg_config);
	reg_config = ((reg_config | _BV(PWR_UP) ) & ~_BV(PRIM_RX));
	nrf24_write_reg(CONFIG,1,(uint8_t *)&reg_config);
	delay_us(150);
}

void nrf24_mode_RX(void) {
	uint8_t reg_config;
	nrf24_read_reg(CONFIG, 1, &reg_config);
	reg_config = reg_config | 3;
	nrf24_write_reg(CONFIG, 1, (uint8_t *)&reg_config);
	delay_us(20);
}

void nrf24_flush_TX(void) {
	spi_habilita();
	spi_write(FLUSH_TX);
	spi_desabilita();
}

void nrf24_flush_RX(void) {
	spi_habilita();
	spi_write(FLUSH_RX);
	spi_desabilita();
}

void nrf24_receiver(uint8_t buffer[]) {
	uint8_t x, value, reg_config;

	while (1) {
		nrf24_CE_HIGH();
		while((nrf24_read_reg(STATUS, 1, &reg_config) & (1 << 6)) == 0);

		nrf24_CE_LOW();
		spi_habilita();
		spi_write(R_RX_PAYLOAD);
		
		for (x = 0; x < MESSAGE_SIZE; x++) {
			buffer[x] = spi_write(0xff);
		}

		nrf24_flush_RX();

		value = nrf24_read_reg(STATUS, 1, &reg_config);
		value = value | (1 << 6);

		nrf24_write_reg(STATUS, 1, &value);
		nrf24_CE_HIGH();

		uint8_t crc = 0xff;

		for (x = 0; x < MESSAGE_SIZE-1; x++) {
			crc = crc+buffer[x];
		}

		if (crc == buffer[MESSAGE_SIZE-1]) return;
	}
}

void nrf24_transmitter(uint8_t buffer[]) {
	uint8_t x, status, reg_config;
   
   	nrf24_mode_TX();
	nrf24_flush_TX();
	uint8_t crc = 0xff;
	
	for (x = 0; x < MESSAGE_SIZE-1; x++) {
		crc=crc+buffer[x];
	}

	buffer[MESSAGE_SIZE-1] = crc;

	spi_habilita();
	spi_write(W_TX_PAYLOAD);

	for (x = 0; x < MESSAGE_SIZE; x++) {
		spi_write(buffer[x]);
	}

	spi_desabilita();
	
	nrf24_CE_HIGH();
	delay_us(20);

	nrf24_CE_LOW();
	delay_us(20);

	while  ((nrf24_read_reg(STATUS, 1, &reg_config) & (1 << 5)) == 0);

	status = nrf24_read_reg(STATUS, 1, &reg_config); 
	status = (status | (1 << 5));

	nrf24_write_reg(STATUS, 1, (uint8_t *)&status);
	nrf24_mode_RX();
}