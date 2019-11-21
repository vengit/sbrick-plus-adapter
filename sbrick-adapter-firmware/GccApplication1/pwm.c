#include "pwm.h"

void pwm_init_old(void)
{
    //TOCC2 = 00 -> OC0B
    TOCPMSA1=0;
    TOCPMSA0=0;
    // TOCC1 output enable
    TOCPMCOE|= _BV(TOCC2OE);
    // Timer 0 8bit FASTPWM MODE
    TCCR0A |= _BV(WGM01);
    TCCR0A |= _BV(WGM00);
    // Timer 0 clk /
    TCCR0B |= _BV(CS00);
    // Clear OC0A on Compare Match when up-counting. Set OC0A on Compare Match when down-counting.
    //TCCR0A |= _BV(COM0A1);
    // Clear OC0B on Compare Match when up-counting. Set OC0B on Compare Match when down-counting.
    //TCCR0A |= _BV(COM0B1);
    OCR0A = 0;
    OCR0B = 0;
    
    DDRA &= ~(_BV(PA7)); // IN
    DDRA |= _BV(PA3); // OUT
    PORTA &= ~(_BV(PA3)); // SET 0

}

void pwm_init(void)
{
    //TOCC2 = 00 -> OC0B
    TOCPMSA1=0;
    TOCPMSA0=0;
    // TOCC1 output enable
    TOCPMCOE|= _BV(TOCC2OE);
    // Timer 0 8bit phase corrected PWM MODE
    //TCCR0A |= _BV(WGM01);
    TCCR0A |= _BV(WGM00);
    // Timer 0 clk /
    TCCR0B |= _BV(CS00);
    // Clear OC0A on Compare Match when up-counting. Set OC0A on Compare Match when down-counting.
    //TCCR0A |= _BV(COM0A1);
    // Clear OC0B on Compare Match when up-counting. Set OC0B on Compare Match when down-counting.
    //TCCR0A |= _BV(COM0B1);
    OCR0A = 0;
    OCR0B = 0;
    
    DDRA &= ~(_BV(PA7)); // IN
    DDRA |= _BV(PA3); // OUT
    PORTA &= ~(_BV(PA3)); // SET 0

}

void pwm_set(uint8_t val)
{
    if (val == 0) {
        TCCR0A &= ~(_BV(COM0B1));
        PORTA &= ~(_BV(PA3)); // SET 0
        
    } else if (val == 255) {
        TCCR0A &= ~(_BV(COM0B1));
        PORTA |= _BV(PA3); // SET 1
        
    } else {
        TCCR0A |= _BV(COM0B1);
        OCR0B=val;
    }
}
