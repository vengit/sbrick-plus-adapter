#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

void adc_init();
uint8_t get_adc();
