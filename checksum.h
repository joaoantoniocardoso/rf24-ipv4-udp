#ifndef _CHECKSUM_H
#define _CHECKSUM_H

#include <stdint.h>

int16_t check_csum(int16_t *data, uint16_t dataLenght, uint16_t checkSumByte);
int16_t make_csum(int16_t *data, uint16_t dataLenght, uint16_t checkSumByte);

#endif /* ifndef _CHECKSUM_H */