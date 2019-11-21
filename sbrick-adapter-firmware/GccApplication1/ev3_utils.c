#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ev3_utils.h"
#include "ev3_inline.h"

uint8_t get_short(uint8_t *data, uint8_t rec)
{
    uint8_t len=get_length(data[0]); // len does not contains the 1 byte header
    if (rec+1<=len) {
        return data[rec+1];
    }
    return 0;
}

uint16_t get_int(uint8_t *data, uint8_t rec)
{
    uint8_t len=get_length(data[0]); // len does not contains the 1 byte header
    if (rec*2+2<=len) {
        uint16_t temp=data[rec*2+1];
        return temp+((uint16_t)data[rec*2+2]<<8);
    }
    return 0;
}

uint32_t get_long(uint8_t *data, uint8_t rec)
{
    uint8_t len=get_length(data[0]); // len does not contains the 1 byte header
    if (rec*4+4<=len) {
        uint32_t temp=data[rec*4+1];
        temp+=((uint32_t)data[rec*4+2]<<8);
        temp+=((uint32_t)data[rec*4+3]<<16);
        temp+=((uint32_t)data[rec*4+4]<<24);
        return temp;
    }
    return 0;
}

float get_float(uint8_t *data, uint8_t rec)
{
    uint8_t len=get_length(data[0]); // len does not contains the 1 byte header
    if (rec*4+4<=len) {
        return *((float*)(data+rec*4+1));
    }
    return 0.0;
}

void set_mode(uint8_t *buff, uint8_t mode)
{
    buff[0]=COMMAND_SELECT;
    buff[1]=mode;
    buff[2]=calculate_checksum(buff, 2);
}

void set_speed(uint8_t *buff, uint32_t baud)
{
    buff[0]=COMMAND_SPEED;
    buff[1]=baud & 0xff;
    buff[2]=(baud>>8) & 0xff;
    buff[3]=(baud>>16) & 0xff;
    buff[4]=(baud>>24) & 0xff;
    buff[5]=calculate_checksum(buff, 5);
}
