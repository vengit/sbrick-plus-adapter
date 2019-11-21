#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "capture.h"

uint16_t p_start_time, p_stop_time;
uint16_t p_period;
uint16_t p_duty_raw;
uint8_t p_duty;

uint16_t get_period()
{
    return p_period;
}

uint16_t get_duty_raw()
{
    return p_duty_raw;
}

ISR(TIMER1_COMPA_vect)
{
    uint16_t sample;

    /* slide timeout window forward */
    OCR1A += p_period;

    /* assume 0% */
    sample = 0;

    // check edge select
    if ((TCCR1B & _BV(ICES1)) != _BV(ICES1)) {
        /* 100% */
        sample = p_period-1;
    }

    p_duty_raw=sample;

    return;
}

/**
 * TIMER1_CAPT()
 *
 * capture interrupt.
 */
ISR(TIMER1_CAPT_vect)
{
    uint16_t icr, delta;
    uint8_t tccr1b;

    /*
     * Capture the ICR and then reverse the sense of the capture.
     * These must be done in this order, since as soon as the
     * sense is reversed it is possible for ICR to be updated again.
     */
    /* capture timestamp    */
    icr = ICR1;

    do {
        tccr1b = TCCR1B;
        /* reverse sense */
        TCCR1B = tccr1b ^ _BV(ICES1);

        /*
         * If we were waiting for a start edge, then this is the
         * end/beginning of a period.
         */
        if ((tccr1b & _BV(ICES1)) == _BV(ICES1)) {
            /*
             * Beginning of pulse: Compute length of preceding period,
             * and thence the duty cycle of the preceding pulse.
             */
            /* Length of previous period */
            p_period = icr - p_start_time;
            /* Length of previous pulse */
            delta = p_stop_time - p_start_time;
            /* Start of new pulse/period */
            p_start_time = icr;

            /*
             * Update the timeout based on the new period. (The new period
             * is probably the same as the old, give or take clock drift.)
             * We add 1 to make fairly sure that, in case of competition,
             * the PWM edge takes precedence over the timeout.
             */
            /* Move timeout window    */
            OCR1A = icr + p_period + 1;
            /* Clear in case of race */
            TIFR1 = (1 << OCF1A);

            /*
             * Compute the duty cycle, and store the new reading.
             */
            //p_duty=p_duty_compute(delta, p_period);
            p_duty_raw=delta;

            /*
             * Check for a race condition where a (very) short pulse
             * ended before we could reverse the sense above.
             * If the ICP pin is still high (as expected) OR the IF is
             * set (the falling edge has happened, but we caught it),
             * then we won the race, so we're done for now.
             */
            if ((PINA & _BV(PA7)) || (TIFR1 & _BV(ICF1))) {
                break;
            }
        } else {
            /*
             * Falling edge detected, so this is the end of the pulse.
             * The time is simply recorded here; the final computation
             * will take place at the beginning of the next pulse.
             */
            /* Capture falling-edge time */
            p_stop_time = icr;
            /*
             * If the ICP pin is still low (as expected) OR the IF is
             * set (the transition was caught anyway) we won the race,
             * so we're done for now.
             */
            if ((!(PINA & _BV(PA7))) || (TIFR1 & _BV(ICF1)))
                break;
        }
        /*
         * If we got here, we lost the race with a very short/long pulse.
         * We now loop, pretending (as it were) that we caught the transition.
         * The Same ICR value is used, so the effect is that we declare
         * the duty cycle to be 0% or 100% as appropriate.
         */
    } while (1);

    return;
}

void capture_init(void)
{
    TCCR1A = 0;
    OCR1A = 0;
    TCCR1B = _BV(ICES1) | _BV(CS10);
    TIMSK1 = _BV(ICIE1) | _BV(OCIE1A);
}

void capture_uninit(void)
{
    TCCR1A = 0;
    OCR1A = 0;
    TCCR1B = 0;
    TIMSK1 = 0;
}
