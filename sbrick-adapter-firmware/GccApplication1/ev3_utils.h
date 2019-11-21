#pragma once

#define COL_TYPE 29
#define US_TYPE 30
#define GYR_TYPE 32
#define IR_TYPE 33
#define TILT_TYPE 34
#define MOTION_TYPE 35
#define BOOST_COL_TYPE 37
#define INTERACTIVE_M_TYPE 38


#define SYNC 0b00000000
#define NACK 0b00000010
#define ACK 0b00000100

#define TYPE_MASK 0b11000000
#define LENGTH_MASK 0b00111000
#define MODE_MASK 0b00000111
#define TYPE_SHIFT 6
#define LENGTH_SHIFT 3
#define MODE_SHIFT 0

#define TYPE_SYSTEM 0
#define TYPE_COMMAND 1
#define TYPE_INFO 2
#define TYPE_DATA 3

#define COMMAND_TYPE 0b01000000
#define COMMAND_TYPE_LEN 1

#define COMMAND_MODE 0b01001001
#define COMMAND_MODE_LEN 2

#define COMMAND_SPEED 0b01010010
#define COMMAND_SPEED_LEN 4
#define COMMAND_SELECT 0b01000011
#define COMMAND_SELECT_LEN 1
#define COMMAND_WRITE 0b01000100

#define INFO_NAME 0
#define INFO_RAW 1
#define INFO_PCT 2
#define INFO_SI 3
#define INFO_SYMBOL 4
#define INFO_FORMAT 0x80

#define INFO_TYPE_BUFF_SIZE 16
/*
typedef struct {
    uint8_t name[INFO_TYPE_BUFF_SIZE];
    double raw_l;
    double raw_h;
    double pct_l;
    double pct_h;
    double si_l;
    double si_h;
    uint8_t symbol[INFO_TYPE_BUFF_SIZE];
    uint8_t s_item_num;
    uint8_t t_data_type;
    uint8_t f_digits_to_show;
    uint8_t d_decimals_to_show;
} info_type;
*/

typedef struct {
    uint8_t t_type;
    uint8_t m_modes_supported;
    uint8_t v_modes_to_be_shown;
    uint32_t s_speed;
} sensor_type;

uint8_t get_short(uint8_t *data, uint8_t rec);

uint16_t get_int(uint8_t *data, uint8_t rec);

uint32_t get_long(uint8_t *data, uint8_t rec);

float get_float(uint8_t *data, uint8_t rec);

void set_mode(uint8_t *buff, uint8_t mode);

void set_speed(uint8_t *buff, uint32_t baud);
