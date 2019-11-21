#include <inttypes.h>
#include "ev3_utils.h"
#include "ev3_proc.h"

extern sm_state msm;
extern sensor_type sensor;
#ifndef COMPLETE_CONF
extern uint8_t databuf[18];
#else
extern uint8_t databuf[280];
extern info_type it[10];
#endif

void init_sm_state()
{
    msm.i_counter = 0;
}

uint8_t proc_speed(uint8_t i_byte)
{
    if (msm.i_counter == 0 && i_byte == COMMAND_SPEED) {
        msm.length=get_length(COMMAND_SPEED);
        if (msm.i_counter<18) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter > 0 && msm.i_counter < 5) {
        if (msm.i_counter<18) {
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
        if (msm.i_counter<18) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter == 1) {
        if (msm.i_counter<18) {
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
        if (msm.i_counter<18) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter > 0 && msm.i_counter < 3) {
        if (msm.i_counter<18) {
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
        if (msm.i_counter<18) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.length=get_length(i_byte);
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter > 0 && msm.i_counter < msm.length+1) {
        if (msm.i_counter<18) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter == msm.length+1) {
        msm.i_counter=0;
        if (msm.length+1<18) {
            if(!verify_checksum(msm.buffer,msm.length+1,i_byte)) {
                return 2;
            }
        }
        return 1;
    }
    return 2;
}

uint8_t proc_data(uint8_t i_byte)
{
    if (msm.i_counter == 0 && get_type(i_byte) == TYPE_DATA) {
        if (get_length(i_byte)>8) {
            return 2;
        }
        if (msm.i_counter<18) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.length=get_length(i_byte);
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter > 0 && msm.i_counter < msm.length+1) {
        if (msm.i_counter<18) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter == msm.length+1) {
        msm.i_counter=0;
        if (msm.length+1<18) {
            if(!verify_checksum(msm.buffer,msm.length+1,i_byte)) {
                return 2;
            }
        }
        uint8_t i=0;
        if (msm.i_counter<18) {
            for(;i<msm.length+1;i++) {
                databuf[i]=msm.buffer[i];
            }
        } else {
            for(;i<18;i++) {
               databuf[i]=msm.buffer[i];
            }
        }
        return 1;
    }
    return 2;
}

uint8_t proc_info(uint8_t i_byte)
{
    if (msm.i_counter == 0 && get_type(i_byte) == TYPE_INFO) {
        if (msm.i_counter<18) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.length=get_length(i_byte);
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter > 0 && msm.i_counter < msm.length+2) {
        if (msm.i_counter<18) {
            msm.buffer[msm.i_counter]=i_byte;
        }
        msm.i_counter++;
        return 0;
    } else if (msm.i_counter == msm.length+2) {
        msm.i_counter=0;
#ifndef COMPLETE_CONF
	if (msm.length+2<18) {
	    if(!verify_checksum(msm.buffer,msm.length+2,i_byte)) {
		return 2;
	    }
	}
#else
       uint8_t mode=get_mode(msm.buffer[0]);
       uint8_t i=0;
       switch(msm.buffer[1]) {
          case INFO_NAME:
             for (;(i<(INFO_TYPE_BUFF_SIZE-1))&&(i<msm.length);i++) {
                it[mode].name[i]=msm.buffer[i+2];
             }
             it[mode].name[i]=0;
             break;
	  case INFO_RAW:
	     it[mode].raw_l=*(float*)(msm.buffer+2);
	     it[mode].raw_h=*(float*)(msm.buffer+6);
	     break;
	  case INFO_PCT:
	     it[mode].pct_l=*(float*)(msm.buffer+2);
	     it[mode].pct_h=*(float*)(msm.buffer+6);
	     break;
	  case INFO_SI:
	     it[mode].si_l=*(float*)(msm.buffer+2);
	     it[mode].si_h=*(float*)(msm.buffer+6);
	     break;
	  case INFO_FORMAT:
	     it[mode].s_item_num=msm.buffer[2];
	     it[mode].t_data_type=msm.buffer[3];
	     it[mode].f_digits_to_show=msm.buffer[4];
	     it[mode].d_decimals_to_show=msm.buffer[5];
	     break;
	  case INFO_SYMBOL:
             for (;(i<(INFO_TYPE_BUFF_SIZE-1))&&(i<msm.length);i++) {
                it[mode].symbol[i]=msm.buffer[i+2];
             }
             it[mode].symbol[i]=0;
             break;
	
	  default:
	     return 2;
       }
#endif
        return 1;
    }
    return 2;
}

