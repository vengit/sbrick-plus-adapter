#include <avr/io.h>
#include <avr/interrupt.h>
#incluide <util/delay.h>
#include <inttypes.h>
#include "ser.h"

#define LED PB5
#define PWML PD6

void printInt(int n)
{
    ser_write(((n/100)%10)+'0');
    ser_write(((n/10)%10)+'0');
    ser_write(n%10+'0');
    ser_write('\n');
}

void pwmT0A(int val)
{
    OCR0A = val;
}

int main(void)
{
    sei();
    TCCR0A |= _BV(WGM01);
    TCCR0A |= _BV(WGM00);
    TCCR0B |= _BV(CS01);
    TCCR0B |= _BV(CS00);
    DDRD |= _BV(PWML);
    TCCR0A |= _BV(COM0A1);

    int i=0;
    DDRB |= _BV(LED);
    ser_begin(2400);
    uint8_t c=0;
    PORTB |= _BV(LED);
    while (1) {
        _delay_ms(100);

        pwmT0A(c);
        c+=8;
        delay_ms(100);
        pwmT0A(0);

        if (ser_available()) {        
            PORTB &= ~(_BV(LED));
            i=ser_read();
            printInt(i);
        }
    }
}
