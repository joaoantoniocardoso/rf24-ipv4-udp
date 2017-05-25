#ifndef _IPV4_H
#define _IPV4_H

#include <Arduino.h>
#include <stdint.h>
#include "checksum.h"

//typedef unsigned char uint8_t
//typedef unsigned int uint16_t

#define IPV4_VERSION                4
#define IPV4_IHL                    5
#define IPV4_TOS                    0         
#define IPV4_MIN_LEN                20
#define IPV4_MAX_LEN                576
#define IPV4_ID                     0
#define IPV4_FLAGS_DONT_FRAGMENT    0b010
#define IPV4_FLAGS_DO_FRAGMENT      0b001
#define IPV4_FRAG_OFFSET            0
#define IPV4_TTL_MIN                1
#define IPV4_TTL_MAX                128
#define IPV4_PROTO_TCP              0x06

struct iphdr{
    uint8_t version : 4;
    uint8_t ihl : 4;
    uint8_t tos;
    uint16_t len;
    uint16_t id;
    uint16_t flags : 3;
    uint16_t frag_offset : 13;
    uint8_t ttl;
    uint8_t proto;
    uint16_t csum;
    uint32_t saddr;
    uint32_t daddr;
} __attribute__((packed));

struct ippkg{
    struct iphdr *header;
    uint8_t *data;
} __attribute__((packed));

void mkHeader(
    struct iphdr *header, 
    uint32_t saddr, 
    uint32_t daddr);

void makePackage(
    struct iphdr *header, 
    uint8_t *data, 
    uint8_t dataLenght, 
    struct ippkg *package);

uint32_t inet_addr(const char *strAddr);

#endif /* ifndef _IPV4_H */
