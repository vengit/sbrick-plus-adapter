#pragma once

#include <inttypes.h>
#include "mode_conf.h"
#include "ev3_utils.h"

typedef struct {
    uint8_t length;    //length of the type
    uint8_t i_counter; //invocation counter
    uint8_t buffer[18]; //data buffer to store data 128+2 headers; 1 checksum not stored
} sm_state;

void init_sm_state();

uint8_t proc_speed(uint8_t i_byte);
uint8_t proc_type(uint8_t i_byte);
uint8_t proc_mode(uint8_t i_byte);
uint8_t proc_other_cmd(uint8_t i_byte);
uint8_t proc_info(uint8_t i_byte);

uint8_t proc_data(uint8_t i_byte);
