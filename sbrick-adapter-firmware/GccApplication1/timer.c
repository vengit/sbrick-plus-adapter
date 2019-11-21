#include "timer.h"

/* Set up TIMER2. Division: 1024 */
void timer_init(void)
{
    TCCR2A = 0;
    TCCR2B = _BV(CS20) | _BV(CS22);
}

void timer_set(uint16_t val)
{
    TCNT2=val;
}

uint16_t timer_get()
{
    return TCNT2;
}

void timer_uninit(void)
{
    TCCR2A = 0;
    TCCR2B = 0;
}
