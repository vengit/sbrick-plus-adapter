#include <avr/eeprom.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <avr/fuse.h>
#include <avr/lock.h>
#include <stdio.h>
#include <stdlib.h>
#include "capture.h"
#include "ser.h"
#include "ser1.h"
#include "pwm.h"
#include "timer.h"
#include "adc.h"
#include "main_fm.h"
#include "main_uart.h"

#include <util/delay.h>

FUSES =
{
    /* 8MHz internal RC oscillator, no clock output, no clock divison */
    .low = 0xE2,

    /* BOD @ 2.7V, serial programming enabled*/
    .high = 0xDD,

    /* 32KHz ULPOSC (not used), BOD active on all power levels */
    .extended = 0xF5
};

/* No further reading or writing */
LOCKBITS = LB_MODE_3;

/* F_CPU/1024/100 = 81 */
#define TIMER_10MS 81

/* F_CPU/1024/100 = 2.025 */
#define TIMER_025MS 2

/* Atmel ICE calibration frequency on MOSI pin */
#define ISP_FREQ 32100.0

/* Measure this much caluibration clock cycles */
#define CAL_CLOCK_TICKS 100.0

/* With perfect calibration we should count this many clock ticks */
#define TARGET_TICKS (F_CPU/ISP_FREQ*CAL_CLOCK_TICKS)

/* How many times check the MOSI pin for calibration signal.
 * Should be a 100ms-ish amount. A loop has a 5us wait. */
#define MAX_WAIT_FOR_CALIBRATION 20000

/* MOSI, also the calibration frequency reference pin */
#define MOSI_PIN PA6

/* MISO, signals back to the programmer during calibration */
#define MISO_PIN PA5

/* Save calibration info here */
#define EEPROM_CALIBRATION_ADDRESS 0x00

enum main_mode {INIT, INIT_VERIFY, START_OPERATING, OPERATING};

typedef enum main_mode main_mode_t;

main_mode_t mode=INIT;

uint8_t sub_mode=EV3TOUCH_SM;

extern uint16_t p_period;
extern uint16_t p_duty_raw;

/* Returns 1 if the parameters mean we're being initialized */
uint8_t init_freq(uint16_t p, uint8_t d)
{
    if ((p > (INIT_F-DELTA_F)) && (p < (INIT_F+DELTA_F))) {
        if (d>1 && d<254) {
            return 1;
        }
    }
    return 0;
}

uint8_t op_freq(uint16_t p, uint8_t d)
{
    if ((p > (OP_F-DELTA_FH)) && (p < (OP_F+DELTA_FH))) {
            return 1;
    }
    return 0;
}

uint8_t get_submode(uint8_t d)
{
    if ((d<R_MIN) || (d>R_MAX)) {
        return 255;
    } 
    return (d-R_MIN)/R_LANE;
    
}

void debug_flag_init()
{    
        DDRB |= _BV(DDB2);
}
    
void debug_flag_set(uint8_t val)
{
    if (val) {
        PORTB |= _BV(PB2);
    } else {
        PORTB &= ~(_BV(PB2)); // SET 0
    }
}

uint8_t scale(uint8_t val)
{
    uint16_t raw=val;
    return (uint8_t)(raw*8/10+R_MIN);
}

/* Measure the current ticks/calibration cycle
 * and return the difference from the ideal
 */
float tick_difference()
{
    uint8_t high_count = 0;

    /* start counter 0 to count clock ticks */
    TCNT0 = 0;
    TCCR0B = _BV(CS00);

    for (int i = 0; i < CAL_CLOCK_TICKS; i++) {
        /* check for overflow */
        if (TIFR0 & _BV(TOV0)) { TIFR0 |= _BV(TOV0); high_count++; }
        /* wait for rising edge */
        loop_until_bit_is_set(PINA, MOSI_PIN);
        /* check for overflow */
        if (TIFR0 & _BV(TOV0)) { TIFR0 |= _BV(TOV0); high_count++; }
        /* wait for falling edge */
        loop_until_bit_is_clear(PINA, MOSI_PIN);
    }

    /* stop counter */
    TCCR0B &= ~_BV(CS00);

    /* check for overflow */
    if (TIFR0 & _BV(TOV0)) { TIFR0 |= _BV(TOV0); high_count++; }

    /* subtract a few ticks because it takes a few instructions to respond */
    float ticks = ((((uint16_t)high_count)<<8 ) | TCNT0) - 2;

    return ticks - TARGET_TICKS;
}

static FILE mystdout = FDEV_SETUP_STREAM(ser1_putchar, NULL, _FDEV_SETUP_WRITE);

/* Measure a ~32 KHz reference signal on MOSI
 * and adjust the OSCCAL0 register with
 * gradient descent */
void calibrate()
{
    /* This is the best distance from TARGET_TICKS. Set it to a large number. */
    float td_best = UINT16_MAX;

    /* Stores the most precise OSCCAL0 value (for which td_best was measured)
     * Start our setup from the factory value and slowly inch towards the optimal value. */
    uint8_t osccal_best = OSCCAL0;

    for(;;) {
        /* Measure how far we are from TARGET_TICKS */
        float td = tick_difference();

        if (abs(td) < td_best) {
            /* This is better than last time */
            td_best = abs(td);
            osccal_best = OSCCAL0;
        } else {
            /* We're not progressing, quit calibration */
            OSCCAL0 = osccal_best;
            break;
        }

        if (td > 0) {
            /* Too many ticks, clock runs too fast */
            if (OSCCAL0 == 0) {
                break; /* Hit the wall */
            }
            /* Set clock and wait a little for the clock to stabilize.
             * The clock is not stable yet, this is not precise. */
            OSCCAL0 -= 1;
            _delay_us(100);
        } else if (td < 0) {
            /* Too few ticks, clock runs too slow */
            if (OSCCAL0 == 255) {
                break; /* Hit the wall */
            }
            /* Set clock and wait a little for the clock to stabilize.
             * The clock is not stable yet, this is not precise. */
            OSCCAL0 += 1;
            _delay_us(100);
        } else {
            /* When the ticks are just right */
            break;
        }
    }

    /* store fresh, calibrated OSCCAL0 value in eeprom */
    eeprom_busy_wait();
    eeprom_update_byte(0x00, OSCCAL0);
    eeprom_busy_wait();

    /* toggle MISO line 8 times to signal success */
    for (int i = 0; i < 8; i++) {
        /* wait for falling edge */
        loop_until_bit_is_clear(PINA, MOSI_PIN);
        /* toggle MISO */
        PINA |= _BV(MISO_PIN);
        /* wait for rising edge */
        loop_until_bit_is_set(PINA, MOSI_PIN);
    }
}

/* Load the RC oscillator calibration value from the
 * configured EEPROM location. */
void load_calibration()
{
    uint8_t cal = eeprom_read_byte(EEPROM_CALIBRATION_ADDRESS);
    if (cal == 0xff) {
        /* The EEPROM looks like it's not programmed. Don't change OSCCAL0. */
        return;
    }
    OSCCAL0 = cal;
}

int calibration_clock_present()
{
    uint32_t i = 0;

    while (bit_is_clear(PINA, MOSI_PIN)) {
        _delay_us(5); /* Clock is uncalibrated might take 2-10us */
       i++;
       if (i > MAX_WAIT_FOR_CALIBRATION) {
           return 0;
       }
    }

    while (bit_is_set(PINA, MOSI_PIN)) {
        _delay_us(5); /* Clock is uncalibrated might take 2-10us */
        i++;
       if (i > MAX_WAIT_FOR_CALIBRATION) {
           return 0;
       }
    }

    return 1;
}

/**
 * Entry point
 */
int main(void)
{
    /* Make printf() work */
    stdout = &mystdout;

    /* set MISO as output */
    DDRA |= _BV(MISO_PIN);

    /* set MOSI as input */
    DDRA &= ~_BV(MOSI_PIN);

    /* enable pull-up resistor on MOSI */
    PUEA |= _BV(MOSI_PIN);

    /* set MISO high to signal the programmer that we're ready */
    PORTA |= _BV(MISO_PIN);

    /* Check if we're receiving a clibration clock signal */
    if (calibration_clock_present()) {
        /* No, there must be a calibration signal present */
        calibrate();
    } else {
        /* Yes, there was a timeout, use previously saved or factory value */
        load_calibration();
    }

    /* Continue with the calibrated, loaded, or factory OSCCAL0 */

    ser_begin(2400);
    //ser_begin(115200);
    //for (;;) ser_write(0b10101010);
    ser_begin1(115200);

    capture_init();
    timer_init();
    adc_init();
    pwm_init();
    debug_flag_init();
    pwm_set(0);
    timer_set(0);
    debug_flag_set(0);
    sei();

    uint8_t error_cnt=0;
    uint8_t error_state=0;
    uint8_t error_len=0;

    uint8_t tmp=0;
    uint16_t start_time=0;

    /* calibration check */
    // pwm_set(127); for(;;);

    /********DEBUG SETTINGS ***************/
    // mode = OPERATING;
    // pwm_set(R_MIN);
    // start_time = timer_get();
    // sub_mode = UARTMODE0_SM;
    //***************************************
    debug_flag_set(0);

    for (;;)
    {
        /*
         * Fetch the latest reading and display it on the LEDs.
         */        
        uint16_t p=p_period;
        uint8_t d=get_duty(p_period, p_duty_raw);

        /*if (error_len>1) {
            debug_flag_set(1);    
        } else {
            debug_flag_set(0);
        }*/
        //DEBUG: ser_write1(p>>8);
        //DEBUG: ser_write1(p&0xff);
        switch (mode) {
        case INIT:
            if (init_freq(p, d)) {
                init_main_uart();
                ser_begin(2400);
                mode=INIT_VERIFY;
                sub_mode=get_submode(d);
            }
            start_time=timer_get();
            pwm_set(0);
            break;
        case INIT_VERIFY:
            //debug_flag_set(0);
            if (init_freq(p, d) && sub_mode==get_submode(d)) {
                if ((timer_get()-start_time)>(TIMER_10MS*15)) {
                    start_time=timer_get();
                    mode=START_OPERATING;
                    pwm_set(R_MIN);
                        
                }
            } else {
                mode=INIT;
                pwm_set(0);
            };
            break;
        case START_OPERATING:
            if ((timer_get()-start_time)>TIMER_10MS*50) {
                if (op_freq(p, d)) {
                    mode=OPERATING;
                    start_time=timer_get();
                } else {
                    mode=INIT;
                }
            }
            break;
        case OPERATING:
            if (timer_get()-start_time>TIMER_10MS*10) {
                if (error_cnt>40) {
                    mode=INIT;
                    pwm_set(0);
                    //DEBUG ser_write1('I');
                }
                start_time=timer_get();
                error_cnt=0;
                error_state=0;
            }
            if (!op_freq(p, d)) {
                if (error_state==0) {
                    error_state=1;
                    error_len=1;
                } else {
                    error_len++;
                }
            } else {
                if (error_state==1) {
                    error_state=0;
                    if (error_len>1) {
                        error_cnt++;
                        error_len=0;
                    }
                }
            }
            switch(sub_mode) {
            case EV3TOUCH_SM:
                if ((EV3PIN6 & _BV(EV3P6))==_BV(EV3P6)) {
                    tmp=R_MAX;
                } else {
                    tmp=R_MIN;
                }
                pwm_set(tmp);
                break;
            case NXTTOUCH_SM:
                tmp=get_adc();
                if (tmp<128) {
                    pwm_set(R_MAX);
                } else {
                    pwm_set(R_MIN);
                }
                break;
            case NXTLIGHT_SM:
                tmp=scale(get_adc());
                pwm_set(tmp);
                break;
            case UARTMODE0_SM:
            case UARTMODE1_SM:
            case UARTMODE2_SM:
            case UARTMODE3_VM:
            case UARTMODE4_VM:
            case UARTMODE5_VM:
            case UARTMODE6_VM:
                main_uart();
                break;
            }
            break;
        }
    }
}
