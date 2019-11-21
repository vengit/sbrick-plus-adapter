#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include "ev3_utils.h"
#include "ev3_proc.h"
#include "ev3.h"

extern state_type ms;
extern sm_state msm;
extern sensor_type sensor;
extern uint8_t databuf[BUFFSIZE];

#include "ev3_inline_mem.h"

void process_content(uint8_t i_byte)
{
    if ((ms == DATAREADY) && i_byte == SYNC) {
        ms = DATASYNC;
    } else if ((ms == DATASYNC || ms == DATAPREP || ms == DATAREADY) && get_type(i_byte) == TYPE_DATA) {
        //printf("SYNC at %d/%d\n",i,get_length(serRead));
        if(proc_data(i_byte)==0){
            ms = DATA;
        } else {
            ms = STOP;
        }
    } else if (ms == DATA) {
        uint8_t res=proc_data(i_byte);
        if(res==2) {
            ms = STOP;
        } else if(res==1) {
            ms = DATAREADY;
        }
    } else {
        //ms=STOP;
    }
}

void process_header(uint8_t i_byte)
{
    //this hack needs as lego sensor sends sometime checksum
    if (ms == SSYNC) {
        if (i_byte == 0xff) {
            ms=READY;
            return;
        } else {
            ms=READY;
        }
    }
    if (ms == READY && i_byte == SYNC) {
        init_sm_state();
        ms = SSYNC;
    } else if (ms == READY && i_byte == ACK) {
        ms = DATAPREP;
    } else if (ms == READY && i_byte == COMMAND_MODE) {
        if(proc_mode(i_byte)==0){
            ms = MODE;
        } else {
            ms = STOP;
        }
    } else if (ms == MODE) {
        uint8_t res=proc_mode(i_byte);
        if(res==2) {
        ms = STOP;
        } else if(res==1) {
        ms = READY;
        }
    } else if (ms == READY && i_byte == COMMAND_TYPE) {
        if(proc_type(i_byte)==0){
        ms = TYPE;
        } else {
        ms = STOP;
        }
    } else if (ms == TYPE) {
        uint8_t res=proc_type(i_byte);
        if(res==2) {
        ms = STOP;
        } else if(res==1) {
        ms = READY;
        }
    } else if (ms == READY && i_byte == COMMAND_SPEED) {
        if(proc_speed(i_byte)==0){
        ms = SPEED;
        } else {
        ms = STOP;
        }
    } else if (ms == SPEED) {
        uint8_t res=proc_speed(i_byte);
        if(res==2) {
        ms = STOP;
        } else if(res==1) {
        ms = READY;
        }
    } else if (ms == READY && get_type(i_byte) == TYPE_COMMAND) {
        if(proc_other_cmd(i_byte)==0){
            ms = OTHER_CMD;
        } else {
            ms = STOP;
        }
    } else if (ms == OTHER_CMD) {
        uint8_t res=proc_other_cmd(i_byte);
        if(res==2) {
            ms = STOP;
            } else if(res==1) {
            ms = READY;
        }
    } else if (ms == READY && get_type(i_byte) == TYPE_INFO) {
        if(proc_info(i_byte)==0){
        ms = INFO;
        } else {
        ms = STOP;
        }
    } else if (ms == INFO) {
        uint8_t res=proc_info(i_byte);
        if(res==2) {
            ms = STOP;
        } else if(res==1) {
            ms = READY;
        }
    }
}
