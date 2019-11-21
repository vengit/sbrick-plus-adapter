#include <inttypes.h>
#include "ev3_utils.h"
#include "main_fm.h"
#include "uart_calc.h"

extern uint8_t sub_mode;
extern sensor_type sensor;

uint8_t get_sensor_data(uint8_t* databuf)
{
    uint8_t res=0;
    uint16_t i=0;
	uint32_t l=0;
    switch(sensor.t_type) {
    case US_TYPE:
        if (sub_mode==UARTMODE0_SM) {
            i=get_int(databuf, 0);
            res=i/10;
        } else if (sub_mode==UARTMODE1_SM) {
            i=get_int(databuf, 0);
            res=i/4;
        } else if (sub_mode==UARTMODE2_SM) {
            i=get_short(databuf, 0);
            if (i==1) {res=255;} else {res=0;}
        }
        break;
    case GYR_TYPE:
        if (sub_mode==UARTMODE0_SM) {
			i=get_int(databuf, 0);
			if (i>=32768) {
				i=65535-i+1;
				res=(360-i%360)*181>>8;
			} else {
				res=i%360*181>>8;
			}
        } else if (sub_mode==UARTMODE3_VM) {
	        i=get_int(databuf, 0);
	        res=i>>8;
        } else if (sub_mode==UARTMODE1_SM) {
            i=get_int(databuf, 0);
            //i=i+0x7f00;
            res=i>>4;
        }
        break;
    case COL_TYPE:
        if (sub_mode==UARTMODE0_SM) {
            res=get_short(databuf, 0);
            res=(res<<1)+(res>>1);
        } else if (sub_mode==UARTMODE1_SM) {
            res=get_short(databuf, 0);
        } else if (sub_mode==UARTMODE2_SM) {
            res=get_short(databuf, 0);
            res=res<<5;
        }
        break;
    case IR_TYPE:
        if (sub_mode==UARTMODE0_SM) {
            res=get_short(databuf, 0);
            res=(res<<1)+(res>>1);
        } else if (sub_mode==UARTMODE1_SM) {
            res=get_short(databuf, 0);
        } else if (sub_mode==UARTMODE4_VM) {
			res=get_short(databuf, 1);			
        } else if (sub_mode==UARTMODE5_VM) {
	        res=get_short(databuf, 2);
        } else if (sub_mode==UARTMODE6_VM) {
		    res=get_short(databuf, 3);
        } else if (sub_mode==UARTMODE2_SM) {
            i=get_short(databuf, 0);
            res=i*23;
        }
        break;
    case MOTION_TYPE:
        if (sub_mode==UARTMODE0_SM) {
            res=get_short(databuf, 0);
            res=res*25;
        } else if (sub_mode==UARTMODE1_SM) {
            res=get_short(databuf, 0);
			res=res*2;
        }
        break;
    case TILT_TYPE:
        if (sub_mode==UARTMODE0_SM) {
            i=get_short(databuf, 0);                                     
			if (i>=128) {
				i=256-i+1;
				res=(360-i*2%360)*181>>8;
				} else {
				res=i*2%360*181>>8;
			}
        } else if (sub_mode==UARTMODE3_VM) {
	        res=get_short(databuf, 1);
        } else if (sub_mode==UARTMODE1_SM) {
            res=get_short(databuf, 0);
            res=res*25;
        } else if (sub_mode==UARTMODE2_SM) {
            res=get_short(databuf, 1);
            res=res*2;
        }
        break;
	case BOOST_COL_TYPE:
		if (sub_mode==UARTMODE0_SM) {
			res=get_short(databuf, 0);
			res=res<<4;
		}
		break;
	case INTERACTIVE_M_TYPE:
	    if (sub_mode==UARTMODE0_SM) {
			res=get_short(databuf, 0);
		} else if (sub_mode==UARTMODE1_SM) {
	        res=get_short(databuf, 0);
        } else if (sub_mode==UARTMODE2_SM) {
	        l=get_long(databuf, 0);
			if (l>=2147483648) {
				l=4294967295-l+1;
				res=(360-l%360)*181>>8;
			} else {
				res=l%360*181>>8;
			}
	        //res=((l+360)%360)>>2;
		}
		break;		
    default:
        res=get_short(databuf, 0);
        break;
    }
    return res;
}
