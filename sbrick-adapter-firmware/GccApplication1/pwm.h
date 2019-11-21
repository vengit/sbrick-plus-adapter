#pragma once

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>

void pwm_init();
void pwm_set(uint8_t val);
