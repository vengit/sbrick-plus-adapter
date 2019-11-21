#pragma once

#include <inttypes.h>
#include "mode_conf.h"
#include "ev3_utils.h"
#include "ev3_proc.h"

typedef enum {TYPE, SPEED, READY, OTHER_CMD, INFO, STOP, DATAPREP, DATA, DATAREADY, DATASYNC, MODE, SSYNC} state_type;
void process_content(uint8_t i_byte);
void process_header(uint8_t i_byte);
