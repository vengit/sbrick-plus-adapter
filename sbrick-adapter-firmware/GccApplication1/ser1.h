#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>

#define SERIAL_TX_BUFFER_SIZE1 8
#define SERIAL_RX_BUFFER_SIZE1 8
#define SERIAL_8N1 0x06

void ser_tx_udr_empty_irq1(void);
void ser_rx_complete_irq1(void);
void ser_begin1(uint32_t baud);
void ser_changebaud1(uint32_t baud);
void ser_flush1();
uint8_t ser_read1(void);
int8_t ser_write1(uint8_t c);
int8_t ser_print1(uint8_t *c);
int8_t ser_available1(void);
void ser_print_int1(uint16_t n);
int ser1_putchar(char c, FILE *stream);
