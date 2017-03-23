#ifndef _IPV4_H
#define _IPV4_H

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
    unsigned char version : 4;
    unsigned char ihl : 4;
    unsigned char tos;
    unsigned int len;
    unsigned int id;
    unsigned int flags : 3;
    unsigned int frag_offset : 13;
    unsigned char ttl;
    unsigned char proto;
    unsigned int csum;
    unsigned long saddr;
    unsigned long daddr;
} __attribute__((packed));

struct ippkg{
    struct iphdr *header;
    unsigned char *data;
} __attribute__((packed));

void mkHeader(      
        struct iphdr *header, 
        unsigned int saddr, 
        unsigned int daddr);
void makePackage(
        struct iphdr *header, 
        unsigned char *data, 
        unsigned dataLengh, 
        struct ippkg *package);

#endif /* ifndef _IPV4_H */
