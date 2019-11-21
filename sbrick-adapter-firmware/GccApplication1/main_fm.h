#pragma once


/* "Do not calibrate to more than 8.8MHz if EEPROM or Flash is
 * to be written. Otherwise, the EEPROM or Flash write may
 * fail." */

// #define F_CPU 7372800UL
   #define F_CPU 8294400UL
// #define F_CPU 9216000UL

#include <util/delay.h>

#define INIT_F 11840
#define NORM_F 7870
#define OP_F 512

#define DELTA_F 550
#define DELTA_FH 100
#define R_MIN 20
#define R_MAX 220
#define R_LANE 20

#define EV3TOUCH_SM 0
#define NXTTOUCH_SM 1
#define NXTLIGHT_SM 2
#define UARTMODE0_SM 3
#define UARTMODE1_SM 4
#define UARTMODE2_SM 5
#define UARTMODE3_VM 6
#define UARTMODE4_VM 7
#define UARTMODE5_VM 8
#define UARTMODE6_VM 9

#define EV3P1 PB1
#define EV3P2 PB0
#define EV3P5 PA1
#define EV3P6 PA2

#define EV3PIN1 PINB
#define EV3PIN2 PINB
#define EV3PIN5 PINA
#define EV3PIN6 PINA

uint8_t scale(uint8_t val);
