#include "checksum.h"

/**
 * @brief Checks a 16bits Integer check sum w/ one's complement
 * @param *data is a pointer to the prealocated data
 * @param dataLenght is the lenght of data
 * @param checkSumByte indicates where the check sum byte is in the data
 */                                 
int16_t check_csum(int16_t *data, uint16_t dataLenght, uint16_t checkSumByte)
{
    checkSumByte--;
    int16_t sum = 0;
    int16_t *data_start = data;     // keeps the pointer reference
    for(; dataLenght; dataLenght--){
        // ignores the checkSumByte for computing the sum
        if(dataLenght != checkSumByte){
            sum += *data++;
            if(sum & 0xFFFF0000) sum = (sum & 0xFFFF) +1; // ones' complement
        }else data++;
    }
    data = data_start;          // takes back the pointer reference

    if(data[checkSumByte] == sum) return 0;
    else return -1;
}

/**
 * @brief Makes a 16bits Integer check sum w/ one's complement
 * @param *data is a pointer to the prealocated data
 * @param dataLenght is the lenght of data
 * @param checkSumByte indicates where the check sum byte is in the data
 */                                 
int16_t make_csum(int16_t *data, uint16_t dataLenght, uint16_t checkSumByte)
{
    checkSumByte--;
    int16_t sum = 0;
    int16_t *data_start = data;     // keeps the pointer reference
    for(; dataLenght; dataLenght--){
        sum += *data++;
        if(sum & 0xFFFF0000) sum = (sum & 0xFFFF) +1; // ones' complement
    }
    data = data_start;          // takes back the pointer reference

    // Put the sum at checkSumByte of data
    data[checkSumByte] = sum;

    return sum;
}