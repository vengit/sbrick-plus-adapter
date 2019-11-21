#include <inttypes.h>
#include "ev3_utils.h"
#include "ev3_proc.h"

extern sm_state msm;
extern sensor_type sensor;
extern uint8_t databuf[BUFFSIZE];

#include "ev3_inline_mem.h"

uint8_t proc_speed(uint8_t i_byte)
{
    if (msm.i_counter == 0 && i_byte == COMMAND_SPEED) {
        msm.length=get_length(COMMAND_SPEED);
        if (msm.i_counter<BUFFSIZE) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter > 0 && msm.i_counter < 5) {
        if (msm.i_counter<BUFFSIZE) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter == 5) {
        msm.i_counter=0;
        if(!verify_checksum(msm.buffer,5,i_byte)) {
            return 2;
        }
        sensor.s_speed=msm.buffer[4];
        sensor.s_speed=(sensor.s_speed<<8)+msm.buffer[3];
        sensor.s_speed=(sensor.s_speed<<8)+msm.buffer[2];
        sensor.s_speed=(sensor.s_speed<<8)+msm.buffer[1];
        return 1;
    }
    return 2;
}

uint8_t proc_type(uint8_t i_byte)
{
    if (msm.i_counter == 0 && i_byte == COMMAND_TYPE) {
        if (msm.i_counter<BUFFSIZE) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter == 1) {
        if (msm.i_counter<BUFFSIZE) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter == 2) {
        msm.i_counter=0;
        if(!verify_checksum(msm.buffer,2,i_byte)) {
            return 2;
        }
        sensor.t_type=msm.buffer[1];
        return 1;
    }
    return 2;
}

uint8_t proc_mode(uint8_t i_byte)
{
    if (msm.i_counter == 0 && i_byte == COMMAND_MODE) {
        if (msm.i_counter<BUFFSIZE) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter > 0 && msm.i_counter < 3) {
        if (msm.i_counter<BUFFSIZE) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter == 3) {
        msm.i_counter=0;
        if(!verify_checksum(msm.buffer,3,i_byte)) {
            return 2;
        }
        sensor.m_modes_supported=msm.buffer[1];
        sensor.v_modes_to_be_shown=msm.buffer[2];
        return 1;
    }
    return 2;
}

uint8_t proc_other_cmd(uint8_t i_byte)
{
    if (msm.i_counter == 0 && get_type(i_byte) == TYPE_COMMAND) {
        if (msm.i_counter<BUFFSIZE) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.length=get_length(i_byte);
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter > 0 && msm.i_counter < msm.length+1) {
        if (msm.i_counter<BUFFSIZE) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter == msm.length+1) {
        msm.i_counter=0;
        if (msm.length+1<BUFFSIZE) {
            if(!verify_checksum(msm.buffer,msm.length+1,i_byte)) {
                return 2;
            }
        }
        return 1;
    }
    return 2;
}

uint8_t proc_info(uint8_t i_byte)
{
    if (msm.i_counter == 0 && get_type(i_byte) == TYPE_INFO) {
        if (msm.i_counter<BUFFSIZE) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.length=get_length(i_byte);
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter > 0 && msm.i_counter < msm.length+2) {
        if (msm.i_counter<BUFFSIZE) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter == msm.length+2) {
        msm.i_counter=0;
        if (msm.length+2<BUFFSIZE) {
            if(!verify_checksum(msm.buffer,msm.length+2,i_byte)) {
                return 2;
            }
        }
        return 1;
    }
    return 2;
}
