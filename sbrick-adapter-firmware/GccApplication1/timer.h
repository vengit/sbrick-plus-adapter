#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

void timer_init(void);
void timer_set(uint16_t val);
uint16_t timer_get();
void timer_uninit(void);
