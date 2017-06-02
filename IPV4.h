#ifndef _IPV4_H
#define _IPV4_H

#include <Arduino.h>
#include <stdint.h>
#include "checksum.h"

#define IPV4_VERSION                4
#define IPV4_IHL                    5
#define IPV4_TOS                    0         
#define IPV4_MIN_LEN                20
#define IPV4_MAX_LEN                576
#define IPV4_ID                     0
#define IPV4_FLAGS_MORE_FRAGMENTS   0b010
#define IPV4_FLAGS_DONT_FRAGMENT    0b001
#define IPV4_FRAG_OFFSET            0
#define IPV4_TTL_MIN                1
#define IPV4_TTL_MAX                128
#define IPV4_PROTO_TCP              0x06

struct __attribute__((packed)) iphdr{
    uint8_t version : 4;
    uint8_t ihl : 4;
    uint8_t tos;
    uint16_t len; 
    uint16_t id;
    uint16_t frag_offset : 13;
    uint16_t flags : 3;
    uint8_t ttl;
    uint8_t proto;
    uint16_t csum;
    uint32_t saddr;
    uint32_t daddr;
};

struct __attribute__((packed)) ippkg{
    struct iphdr *header;
    uint8_t *data;
};

void mkHeader(
    struct iphdr *header, 
    uint32_t saddr, 
    uint32_t daddr);

void makePackage(
    struct iphdr *header, 
    uint8_t *data, 
    uint16_t dataLenght, 
    struct ippkg *package);

void printHeaderAsBytes(struct iphdr *mheader);
void printHeader(struct iphdr *header);

uint32_t inet_addr(const char *strAddr);

#endif /* ifndef _IPV4_H */
