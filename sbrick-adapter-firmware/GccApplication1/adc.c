#include "adc.h"

void adc_init()
{
    ADMUXA |= 0xA; //001010 select ADC10
    ADMUXB = 0; //REFS=0 (Vcc) and GSEL=0 (x1)
    ADCSRA |= _BV(ADEN) |  _BV(ADATE);
    ADCSRB |= _BV(ADLAR);
    ADCSRA |= _BV(ADSC); //start conversion
}

uint8_t get_adc()
{
    return ADCH;
}
