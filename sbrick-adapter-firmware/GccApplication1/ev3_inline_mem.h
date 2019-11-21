/*
 * IncFile1.h
 *
 * Created: 21/12/2017 00:24:27
 *  Author: gaboo
 */ 


#pragma once

#include "ev3_inline.h"

static inline void init_sm_state()
{
	msm.i_counter = 0;
}


static inline uint8_t proc_data(uint8_t i_byte)
{
	if (msm.i_counter == 0 && get_type(i_byte) == TYPE_DATA) {
		if (get_length(i_byte)>8) {
			return 2;
		}
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
		uint8_t i=0;
		if (msm.i_counter<BUFFSIZE) {
			for(;i<msm.length+1;i++) {
				databuf[i]=msm.buffer[i];
			}
			} else {
			for(;i<BUFFSIZE;i++) {
				databuf[i]=msm.buffer[i];
			}
		}
		return 1;
	}
	return 2;
}

