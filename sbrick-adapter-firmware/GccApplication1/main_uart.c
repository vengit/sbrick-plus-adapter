#include <stdio.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include "main_uart.h"
#include "ev3_utils.h"
#include "ev3.h"
#include "ser.h"
//DEBUG #include "ser1.h" 
#include "pwm.h"
#include "timer.h"
#include "main_fm.h"
#include "uart_calc.h"

state_type ms=READY; // state of the UART processing
sm_state msm; // sub state of the UART processing step
sensor_type sensor; // sensor type record
uint8_t databuf[BUFFSIZE]; // stores input data from the sensor

uint8_t cmdbuf[7]; // output command buff for mode change

#include "ev3_inline_mem.h"

uint8_t read_serial_single()
{
    return ser_read();
}

uint8_t write_serial_single(uint8_t outc)
{
    return ser_write(outc);
}

uint8_t write_serial(uint8_t outc[], uint8_t size)
{
    uint8_t res = 0;
    for(uint8_t i = 0; i<size; i++) {
        res=ser_write(outc[i]);
        if (res != 1)
        break;
    }
    return res;
}

uint8_t ser_open()
{
    ser_begin(2400);
    return 0;
}

uint8_t ser_reopen(uint32_t rate)
{
    ser_changebaud(rate);
    return 0;
}

// time base is 128 us
#define TIMER_100MS 780 
#define TIMER_7_2MS 56

uint8_t type_flag=0, mode_flag=0, speed_flag=0; // flags of header infos; all have to be read to start receiving data
uint32_t baud=0; // baud of the communication
uint8_t in_data=0; // 

//int res=0;
uint16_t sensor_read_val=0;
uint16_t timer_startu=0;
uint8_t valid_data=0;
uint8_t first_tick=1;
uint8_t mode_tick=0;
uint8_t data_err_counter=0;
extern uint8_t sub_mode;
uint8_t buff_count=0;

void init_main_uart()
{
    type_flag=0, mode_flag=0, speed_flag=0;
    ms=READY;
    init_sm_state();
    msm.i_counter=0;
    sensor_read_val=0;
    valid_data=0;
    first_tick=1;
    mode_tick=0;
}

void reset_communication()
{
    ser_reopen(2400);
    type_flag=0, mode_flag=0, speed_flag=0;
    valid_data=0;
    first_tick=1;
    mode_tick=0;
    init_sm_state();
    ms=READY;
}

void main_uart()
{
    if (ms==DATASYNC || ms==DATAPREP || ms==DATAREADY || ms==DATA) {
        if (first_tick) {
            if((timer_get()-timer_startu)>TIMER_7_2MS) {  // ~7,2 ms
                write_serial_single(NACK);
                timer_startu=timer_get();
                first_tick=0;
                if (sub_mode!=UARTMODE0_SM || UARTMODE3_VM) {
                    mode_tick=1;
                }
            } 
        } else if (mode_tick) {
            if(timer_get()-timer_startu>TIMER_100MS/10) {
                    if (sub_mode==UARTMODE1_SM || sub_mode==UARTMODE4_VM  || sub_mode==UARTMODE5_VM || sub_mode==UARTMODE6_VM) {
                        set_mode(cmdbuf, 1);
                        write_serial(cmdbuf, 3);
                        write_serial(cmdbuf, 3);
                        write_serial(cmdbuf, 3);
                    } else if (sub_mode==UARTMODE2_SM) {
                        set_mode(cmdbuf, 2);
                        write_serial(cmdbuf, 3);
                        write_serial(cmdbuf, 3);
                        write_serial(cmdbuf, 3);
                    }
                    mode_tick=0;
            }
        } else {
            if(timer_get()-timer_startu>TIMER_100MS) {  // ~100 ms
                if (valid_data==1) {
                    write_serial_single(NACK);
                    timer_startu=timer_get();
                    valid_data=0;
                    data_err_counter=0;
					//DEBUG ser_write1('A');
                } else {
                    timer_startu=timer_get();
                    reset_communication();
					//DEBUG ser_write1('R');
                }
            }
        }
    }
    //Handle serial stuff
    buff_count = ser_available();
    if (buff_count) {
        //DEBUG ser_write1('T');
        in_data=read_serial_single();
        if (ms==DATASYNC || ms==DATAPREP || ms==DATAREADY || ms==DATA) {
            if ((buff_count>10) &&(ms == DATASYNC || ms == DATAPREP || ms == DATAREADY) && get_type(in_data) == TYPE_DATA) {
                int dlen=get_length(in_data);
                for (int i=0; i<=dlen; i++) {
                    read_serial_single();
                }
                in_data=read_serial_single();
                //DEBUG ser_write1('D');
            }
            process_content(in_data);
            if (ms==DATAREADY) {
                valid_data=1;
                sensor_read_val=get_sensor_data(databuf);
                pwm_set(scale(sensor_read_val));
            } else if (ms==DATASYNC) {
                //DEBUG ser_write1('Y');
                //data valid flag set after nack signal in timing loop
            } else if (ms==STOP) {
                //DEBUG ser_write1('S');
                if(data_err_counter<5) {
                    ms=DATAPREP;
                } 
            }
        } else {
            process_header(in_data);
            if (ms == TYPE) {
                    type_flag=1;
            } else if (ms == SPEED) {
                if (speed_flag==0) {
                    set_speed(cmdbuf, 115200);
                    write_serial(cmdbuf, 6);
                    speed_flag=1;
                }
            } else if (ms == MODE){
                mode_flag=1;
            } else if (ms == DATAPREP) {
                if (type_flag && speed_flag) {
                    write_serial_single(ACK);
                    ser_flush();

                    _delay_ms(5);
                    baud=sensor.s_speed;

                    ser_reopen(baud);
                    _delay_ms(1);
                    if (ser_available()) {
                        read_serial_single();
                    }
                    first_tick=1;
                    timer_startu=timer_get();
                } else {
                    type_flag = speed_flag = mode_flag = 0;
                    ms = READY;
                    init_sm_state();
                }
            }
            if (ms == STOP) {
                reset_communication();
            }
        }
    }
}
