#include <glib.h>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>

#include "ev3_utils.h"
#include "ev3_proc.h"
#include "ev3.h"
 
uint8_t full_dat[] = {0, 64, 33, 158, 73, 5, 2, 177, 82, 0, 225, 0, 0, 76};
uint8_t speed_dat[] = {82, 0, 225, 0, 0, 76};
uint8_t type_dat[] = {64, 33, 158};
uint8_t mode_dat[] = {73, 5, 2, 177};
uint8_t data_d2_dat[] = {192, 0, '?'};
uint8_t data_d1_dat[] = {192, 8, '7'};
uint8_t data_dats1[] = {200, 8, 0, 63};
uint8_t dataw_dats1[] = {8, 0, 63, 200, 8, 0, 63};
uint8_t data_dats2[] = {200, 1, 4, 50};
uint8_t data_dati1[] = {211, 12, 0, 208, 255, 11};
uint8_t data_datl1[] = {220, 0, 0, 0, 0,0,0,0,0, 35};
uint8_t data_mode2cmd[] = {67, 2, 190};
uint8_t sync_2_dat[] = {0, 255};
uint8_t ack_dat[] = {4};
uint8_t info_0_0_dat[] = {152, 0, 73, 82, 45, 80, 82, 79, 88, 0, 68};
uint8_t info_0_1_dat[] = {152, 1, 0, 0, 0, 0, 0, 0, 200, 66, 236};
uint8_t info_0_3_dat[] = {152, 3, 0, 0, 0, 0, 0, 0, 200, 66, 238};
uint8_t info_0_4_dat[] = {152, 4, 112, 99, 116, 0, 0, 0, 0, 0, 4};
uint8_t info_0_dat[] = {152, 0, 73, 82, 45, 80, 82, 79, 88, 0, 68, 
                              152, 1, 0, 0, 0, 0, 0, 0, 200, 66, 236,
                              152, 3, 0, 0, 0, 0, 0, 0, 200, 66, 238,
                              152, 4, 112, 99, 116, 0, 0, 0, 0, 0, 4,
                              144, 128, 1, 0, 3, 0, 237};
uint8_t info_1_0_dat[] = {153, 0, 73, 82, 45, 83, 69, 69, 75, 0, 72};
uint8_t info_1_1_dat[] = {153, 1, 0, 0, 200, 194, 0, 0, 200, 66, 231};
uint8_t info_1_3_dat[] = {153, 3, 0, 0, 200, 194, 0, 0, 200, 66, 229};
uint8_t info_1_4_dat[] = {153, 4, 112, 99, 116, 0, 0, 0, 0, 0, 5};
uint8_t info_1_128_dat[] = {145, 128, 8, 0, 3, 0, 229};
uint8_t info_2_0_dat[] = {162, 0, 73, 82, 45, 82, 69, 77, 79, 84, 69, 0, 0, 0, 0, 0, 0, 0, 111};
uint8_t info_2_1_dat[] = {154, 1, 0, 0, 0, 0, 0, 0, 32, 65, 5};
uint8_t info_2_3_dat[] = {154, 3, 0, 0, 0, 0, 0, 0, 32, 65, 7};
uint8_t info_2_4_dat[] = {154, 4, 98, 116, 110, 0, 0, 0, 0, 0, 25};
uint8_t info_2_128_dat[] = {146, 128, 4, 0, 3, 0, 234};
uint8_t info_3_0_dat[] = {155, 0, 73, 82, 45, 82, 69, 77, 45, 65, 100};
uint8_t info_3_1_dat[] = {155, 1, 0, 0, 0, 0, 0, 255, 127, 71, 162};
uint8_t info_3_3_dat[] = {155, 3, 0, 0, 0, 0, 0, 255, 127, 71, 160};
uint8_t info_3_128_dat[] = {147, 128, 1, 1, 6, 0, 234};
uint8_t info_4_0_dat[] = {156, 0, 73, 82, 45, 83, 45, 65, 76, 84, 114};
uint8_t info_4_1_dat[] = {156, 1, 0, 0, 0, 0, 0, 0, 200, 66, 232};
uint8_t info_4_3_dat[] = {156, 3, 0, 0, 0, 0, 0, 0, 200, 66, 234};
uint8_t info_4_4_dat[] = {156, 4, 112, 99, 116, 0, 0, 0, 0, 0, 0};
uint8_t info_4_128_dat[] = {148, 128, 4, 0, 3, 0, 236};
uint8_t info_5_0_dat[] = {157, 0, 73, 82, 45, 67, 65, 76, 0, 0, 26};
uint8_t info_5_1_dat[] = {157, 1,  0,  0,  0,  0,  0, 255, 127, 71, 164};
uint8_t info_5_3_dat[] = {157, 3,  0,  0,  0,  0,  0, 255, 127, 71, 166};
uint8_t info_5_128_dat[] = {149, 128, 2, 1, 5, 0, 236};

state_type ms=READY;
sm_state msm;
sensor_type sensor;

#ifdef COMPLETE_CONF
info_type it[10];
uint8_t databuf[280];
#else
uint8_t databuf[18];
#endif

uint8_t cmdbuf[18];

void reset_sensor_var(){
    sensor_type *sv=&sensor;
    sv->t_type=0;
    sv->s_speed=0;
    sv->m_modes_supported=0;
    sv->v_modes_to_be_shown=0;
}



void test_proc_input(void) {
    init_sm_state();

    reset_sensor_var();
    for(uint8_t i=0;i<14;i++) {
        process_header(full_dat[i]);
    }
    g_assert_true(READY == ms);
    g_assert_cmpint(5, ==, sensor.m_modes_supported);
    g_assert_cmpint(2, ==, sensor.v_modes_to_be_shown);
    g_assert_cmpint(57600, ==, sensor.s_speed);
    g_assert_cmpint(33, ==, sensor.t_type);
}


void test_proc_content(void) {
    memset(databuf, 0, 10);
    init_sm_state();
    ms=DATAPREP;

    process_content(data_dats1[0]);
    g_assert_cmpint(DATA, ==, ms);
    g_assert_cmpint(1, ==, msm.i_counter);
    process_content(data_dats1[1]);
    g_assert_cmpint(DATA, ==, ms);
    g_assert_cmpint(2, ==, msm.i_counter);
    process_content(data_dats1[2]);
    g_assert_cmpint(DATA, ==, ms);
    g_assert_cmpint(3, ==, msm.i_counter);
    process_content(data_dats1[3]);
    g_assert_cmpint(DATAREADY, ==, ms);
    g_assert_cmpint(0, ==, msm.i_counter);
    g_assert_cmpint(data_dats1[0], ==, databuf[0]);
    g_assert_cmpint(data_dats1[1], ==, databuf[1]);
    g_assert_cmpint(data_dats1[2], ==, databuf[2]);
    g_assert_cmpint(0, ==, databuf[3]);

    process_content(data_dats2[0]);
    g_assert_cmpint(DATA, ==, ms);
    g_assert_cmpint(1, ==, msm.i_counter);
    process_content(data_dats2[1]);
    g_assert_cmpint(DATA, ==, ms);
    g_assert_cmpint(2, ==, msm.i_counter);
    process_content(data_dats2[2]);
    g_assert_cmpint(DATA, ==, ms);
    g_assert_cmpint(3, ==, msm.i_counter);
    process_content(data_dats2[3]);
    g_assert_cmpint(DATAREADY, ==, ms);
    g_assert_cmpint(0, ==, msm.i_counter);
    g_assert_cmpint(data_dats2[0], ==, databuf[0]);
    g_assert_cmpint(data_dats2[1], ==, databuf[1]);
    g_assert_cmpint(data_dats2[2], ==, databuf[2]);
    g_assert_cmpint(0, ==, databuf[3]);
}

void test_proc_content_s(void) {
    memset(databuf, 0, 10);
    init_sm_state();
    ms=DATAPREP;

    g_assert_cmpint(1, ==, get_length(data_d1_dat[0]));
    process_content(data_d1_dat[0]);
    g_assert_cmpint(DATA, ==, ms);
    g_assert_cmpint(1, ==, msm.i_counter);
    process_content(data_d1_dat[1]);
    g_assert_cmpint(DATA, ==, ms);
    g_assert_cmpint(2, ==, msm.i_counter);
    process_content(data_d1_dat[2]);
    g_assert_cmpint(DATAREADY, ==, ms);
    g_assert_cmpint(0, ==, msm.i_counter);
    g_assert_cmpint(data_d1_dat[0], ==, databuf[0]);
    g_assert_cmpint(data_d1_dat[1], ==, databuf[1]);
    g_assert_cmpint(0, ==, databuf[2]);

}

void test_proc_content_w(void) {
    memset(databuf, 0, 10);
    init_sm_state();
    ms=DATAPREP;

    process_content(dataw_dats1[0]);
    g_assert_cmpint(DATAPREP, ==, ms);
    g_assert_cmpint(0, ==, msm.i_counter);
    process_content(dataw_dats1[1]);
    g_assert_cmpint(DATAPREP, ==, ms);
    g_assert_cmpint(0, ==, msm.i_counter);
    process_content(dataw_dats1[2]);
    g_assert_cmpint(DATAPREP, ==, ms);
    g_assert_cmpint(0, ==, msm.i_counter);
    process_content(dataw_dats1[3]);
    g_assert_cmpint(DATA, ==, ms);
    g_assert_cmpint(1, ==, msm.i_counter);
    process_content(dataw_dats1[4]);
    g_assert_cmpint(DATA, ==, ms);
    g_assert_cmpint(2, ==, msm.i_counter);
    process_content(dataw_dats1[5]);
    g_assert_cmpint(DATA, ==, ms);
    g_assert_cmpint(3, ==, msm.i_counter);
    process_content(dataw_dats1[6]);
    g_assert_cmpint(DATAREADY, ==, ms);
    g_assert_cmpint(0, ==, msm.i_counter);
    g_assert_cmpint(data_dats1[0], ==, databuf[0]);
    g_assert_cmpint(data_dats1[1], ==, databuf[1]);
    g_assert_cmpint(data_dats1[2], ==, databuf[2]);
    g_assert_cmpint(0, ==, databuf[3]);

}

void test_get_length(void) {
    g_assert_cmpint(2, ==, get_length(COMMAND_MODE));
    g_assert_cmpint(4, ==, get_length(COMMAND_SPEED));
    g_assert_cmpint(1, ==, get_length(COMMAND_SELECT));
    g_assert_cmpint(128, ==, get_length(0b01111100));
}

void test_get_type(void){
    g_assert_cmpint(TYPE_SYSTEM, ==, get_type(SYNC));
    g_assert_cmpint(TYPE_SYSTEM, ==, get_type(ACK));
    g_assert_cmpint(TYPE_COMMAND, ==, get_type(COMMAND_SPEED));
    g_assert_cmpint(TYPE_COMMAND, ==, get_type(COMMAND_WRITE));
    g_assert_cmpint(TYPE_INFO, ==, get_type(0b10011001)); //range of raw sensor reading for mode 1
}

void test_get_mode(void){
    g_assert_cmpint(1, ==, get_mode(0b10011001)); //range of raw sensor reading for mode 1
    g_assert_cmpint(5, ==, get_mode(0b10011101)); //range of raw sensor reading for mode 5
}

void test_get_data_all(void){

    g_assert_cmpuint(8, ==, get_short(data_dats1, 0));
    g_assert_cmpuint(1, ==, get_short(data_dats2, 0));
    g_assert_cmpuint(4, ==, get_short(data_dats2, 1));
    g_assert_cmpuint(1025, ==, get_int(data_dats2, 0));
    g_assert_cmpuint(8, ==, get_int(data_dats1, 0));
    g_assert_cmpuint(4291821580, ==, get_long(data_dati1, 0));
}

void test_set_mode(void){

    set_mode(cmdbuf, 2);
    g_assert_cmpuint(data_mode2cmd[0], ==, cmdbuf[0]);
    g_assert_cmpuint(data_mode2cmd[1], ==, cmdbuf[1]);
    g_assert_cmpuint(data_mode2cmd[2], ==, cmdbuf[2]);
}
void test_checksum(void){
    uint8_t test_data[]={1, 2};
    g_assert_cmpint(252, ==, calculate_checksum(test_data, 2));
}

void test_verify_checksum(void){
    uint8_t test_data[]={1, 2};
    g_assert_cmpint(1, ==, verify_checksum(test_data, 2, 252));
    uint8_t test_data2[]={16, 255};
    g_assert_cmpint(1, ==, verify_checksum(test_data2, 2, 16));
    g_assert_cmpint(0, ==, verify_checksum(test_data2, 2, 15));
}

void test_proc_data(void){
    init_sm_state();

    g_assert_cmpint(0, ==, msm.i_counter);
    uint8_t res=proc_data(data_dats1[0]);
    g_assert_cmpint(0, ==, res);
    g_assert_cmpint(1, ==, msm.i_counter);
    res=proc_data(data_dats1[1]);
    g_assert_cmpint(0, ==, res);
    g_assert_cmpint(2, ==, msm.i_counter);
    res=proc_data(data_dats1[2]);
    g_assert_cmpint(0, ==, res);
    g_assert_cmpint(3, ==, msm.i_counter);
    res=proc_data(data_dats1[3]);
    g_assert_cmpint(1, ==, res);
    g_assert_cmpint(0, ==, msm.i_counter);
    g_assert_cmpint(TYPE_DATA, ==, get_type(databuf[0]));
    g_assert_cmpint(8, ==, databuf[1]);
    g_assert_cmpint(0, ==, databuf[2]);

}



void test_proc_type(void){
    init_sm_state();

    g_assert_cmpint(0, ==, msm.i_counter);
    uint8_t res=proc_type(type_dat[0]);
    g_assert_cmpint(0, ==, res);
    g_assert_cmpint(1, ==, msm.i_counter);
    res=proc_type(type_dat[1]);
    g_assert_cmpint(0, ==, res);
    g_assert_cmpint(2, ==, msm.i_counter);
    res=proc_type(type_dat[2]);
    g_assert_cmpint(1, ==, res);
    g_assert_cmpint(0, ==, msm.i_counter);
    g_assert_cmpint(33, ==, sensor.t_type);

}

void test_proc_mode(void){
    init_sm_state();

    g_assert_cmpint(0, ==, msm.i_counter);
    uint8_t res=proc_mode(mode_dat[0]);
    g_assert_cmpint(0, ==, res);
    g_assert_cmpint(1, ==, msm.i_counter);
    res=proc_mode(mode_dat[1]);
    g_assert_cmpint(0, ==, res);
    g_assert_cmpint(2, ==, msm.i_counter);
    res=proc_mode(mode_dat[2]);
    g_assert_cmpint(0, ==, res);
    g_assert_cmpint(3, ==, msm.i_counter);
    res=proc_mode(mode_dat[3]);
    g_assert_cmpint(1, ==, res);
    g_assert_cmpint(0, ==, msm.i_counter);
    g_assert_cmpint(5, ==, sensor.m_modes_supported);
    g_assert_cmpint(2, ==, sensor.v_modes_to_be_shown);

}

void test_proc_speed(void){

    init_sm_state();

    g_assert_cmpint(0, ==, msm.i_counter);
    uint8_t res=proc_speed(speed_dat[0]);
    g_assert_cmpint(0, ==, res);
    g_assert_cmpint(1, ==, msm.i_counter);
    res=proc_speed(speed_dat[1]);
    g_assert_cmpint(0, ==, res);
    g_assert_cmpint(2, ==, msm.i_counter);
    res=proc_speed(speed_dat[2]);
    g_assert_cmpint(0, ==, res);
    g_assert_cmpint(3, ==, msm.i_counter);
    res=proc_speed(speed_dat[3]);
    g_assert_cmpint(0, ==, res);
    g_assert_cmpint(4, ==, msm.i_counter);
    res=proc_speed(speed_dat[4]);
    g_assert_cmpint(0, ==, res);
    g_assert_cmpint(5, ==, msm.i_counter);
    res=proc_speed(speed_dat[5]);
    g_assert_cmpint(1, ==, res);
    g_assert_cmpint(57600, ==, sensor.s_speed);

}
#ifdef COMPLETE_CONF
void test_proc_info_name(void) {
    init_sm_state();

    g_assert_cmpint(0, ==, msm.i_counter);

    uint8_t res=0;
    for (uint8_t i=0;i<10;i++) {
	res=proc_info(info_0_0_dat[i]);
	g_assert_cmpint(0, ==, res);
	g_assert_cmpint(i+1, ==, msm.i_counter);
    }
    res=proc_info(info_0_0_dat[10]);
    g_assert_cmpint(1, ==, res);
    g_assert_cmpstr(it[0].name,==,"IR-PROX");

}

void test_proc_info_block(void) {
    init_sm_state();

    g_assert_cmpint(0, ==, msm.i_counter);
    uint8_t res=0;
    for (uint8_t i=0;i<50;i++) {
	res=proc_info(info_0_dat[i]);
	if (res==0) {
		g_assert_cmpint(0, <, msm.i_counter);
	} else {
		g_assert_cmpint(0, ==, msm.i_counter);
	}
	//printf("%d", i);
	g_assert_cmpint(2, >, res);
    }
    res=proc_info(info_0_dat[50]);
    g_assert_cmpint(1, ==, res);
    g_assert_cmpstr(it[0].name,==,"IR-PROX");
    g_assert_cmpfloat(it[0].raw_l,==,0.);
    g_assert_cmpfloat(it[0].raw_h,==,100.);
    g_assert_cmpfloat(it[0].si_l,==,0.);
    g_assert_cmpfloat(it[0].si_h,==,100.);
    g_assert_cmpstr(it[0].symbol,==,"pct");
    g_assert_cmpint(it[0].s_item_num,==,1);
    g_assert_cmpint(it[0].t_data_type,==,0);
    g_assert_cmpint(it[0].f_digits_to_show,==,3);
    g_assert_cmpint(it[0].d_decimals_to_show,==,0);

}

#endif
int main(int argc, char *argv[]){

    g_test_init(&argc, &argv, NULL);
    g_test_add_func("/util/test_get_length", test_get_length);
    g_test_add_func("/util/test_get_type", test_get_type);
    g_test_add_func("/util/test_get_mode", test_get_mode);
    g_test_add_func("/util/checksum", test_checksum);
    g_test_add_func("/util/verify_checksum", test_verify_checksum);
    g_test_add_func("/util/test_set_mode", test_set_mode);
    g_test_add_func("/proc/test_proc_type", test_proc_type);
    g_test_add_func("/proc/test_proc_mode", test_proc_mode);
    g_test_add_func("/proc/test_proc_speed", test_proc_speed);
    g_test_add_func("/proc/test_proc_input", test_proc_input);
#ifdef COMPLETE_CONF
    g_test_add_func("/proc/test_proc_info_name", test_proc_info_name);
    g_test_add_func("/proc/test_proc_info_block", test_proc_info_block);
#endif
    g_test_add_func("/proc/test_proc_data", test_proc_data);
    g_test_add_func("/proc/test_get_data_all", test_get_data_all);
    g_test_add_func("/proc/test_proc_content", test_proc_content);
    g_test_add_func("/proc/test_proc_content_w", test_proc_content_w);
    g_test_add_func("/proc/test_proc_content_s", test_proc_content_s);
    return g_test_run();
}

