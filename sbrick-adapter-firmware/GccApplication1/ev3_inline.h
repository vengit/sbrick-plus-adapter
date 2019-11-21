/*
 * IncFile1.h
 *
 * Created: 21/12/2017 00:14:52
 *  Author: gaboo
 */ 


#ifndef INCFILE1_H_
#define INCFILE1_H_



static inline uint8_t calculate_checksum(uint8_t *data, uint8_t l)
{
	uint8_t cks = 0xff;
	uint8_t i=0;
	for (; i<l;i++) {
		cks = cks ^ data[i];
	}
	return cks;
}

static inline uint8_t verify_checksum(uint8_t *data, uint8_t l, uint8_t val)
{
	return calculate_checksum(data, l) == val;
}

static inline uint8_t get_length(uint8_t val)
{
	uint8_t normalized_data = (val & LENGTH_MASK) >> LENGTH_SHIFT;
	return 1 << normalized_data;
}

static inline uint8_t get_type(uint8_t val)
{
	uint8_t normalized_data = (val & TYPE_MASK) >> TYPE_SHIFT;
	return normalized_data;
}

static inline uint8_t get_mode(uint8_t val)
{
	uint8_t normalized_data = (val & MODE_MASK) >> MODE_SHIFT;
	return normalized_data;
}


#endif /* INCFILE1_H_ */