#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

#define SERIAL_TX_BUFFER_SIZE 20
#define SERIAL_RX_BUFFER_SIZE 20
#define SERIAL_8N1 0x06

void ser_tx_udr_empty_irq(void);
void ser_rx_complete_irq(void);
void ser_begin(uint32_t baud);
void ser_changebaud(uint32_t baud);
void ser_flush();
uint8_t ser_read(void);
int8_t ser_write(uint8_t c);
int8_t ser_print(uint8_t *c);
uint8_t ser_available(void);
