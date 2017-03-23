#ifndef _CHECKSUM_H
#define _CHECKSUM_H

int check_csum(int *data, unsigned int dataLenght, unsigned int checkSumByte);
int make_csum(int *data, unsigned int dataLenght, unsigned int checkSumByte);

#endif /* ifndef _CHECKSUM_H */
