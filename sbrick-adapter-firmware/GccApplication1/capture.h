#pragma once

void capture_uninit(void);
void capture_init(void);
uint16_t get_period(void);
uint16_t get_duty_raw(void);

static inline uint8_t get_duty(uint16_t period, uint16_t duty_raw)
{
	uint8_t temp_var=period>>8;
	return duty_raw/temp_var;
}

