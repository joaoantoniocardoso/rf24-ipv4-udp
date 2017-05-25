#include "IPV4.h"

/**
 * @brief Makes the header struct for ipv4
 * @param *header is a pointer to the prealocated header
 * @param saddr is the source ipv4 address
 * @param daddr is the destination ipv4 address
 */
void mkHeader(struct iphdr *header, uint32_t saddr, uint32_t daddr)
{

    (*header).version  = IPV4_VERSION;
    (*header).ihl      = IPV4_IHL;
    (*header).tos      = IPV4_TOS;
    (*header).len      = IPV4_MIN_LEN;
    (*header).id       = 0;
    (*header).flags    = IPV4_FLAGS_DONT_FRAGMENT;
    (*header).frag_offset = 0;
    (*header).ttl      = IPV4_TTL_MIN;
    (*header).proto    = IPV4_PROTO_TCP;
    (*header).csum     = 0;
    (*header).saddr    = saddr;
    (*header).daddr    = daddr;

    (*header).csum = make_csum((int *) header, (*header).len, 10);
}

/**
* @brief Packs the header and the data into a package
* @param *header is a pointer to the prealocated header
* @param *data is a pointer to the prealocated data
* @param dataLenght is the lenght of data
* @param *package is a pointer to the prealocated package
*/ 
void makePackage(struct iphdr *header, uint8_t *data, uint8_t dataLenght, struct ippkg *package)
{
    (*header).len += dataLenght;

    (*package).header = header;
    (*package).data = data;
}

/**
* @brief Translate a dot splited string addr to a 4 bytes (32bits) addr
* @param *strAddr is the char array like "192.168.0.1"
* @ret 4 bytes array. If staddr = "192.168.0.1", it returns {192, 168, 0 ,1};
*/
uint32_t inet_addr(const char *strAddr)
{
    uint8_t a = 0, b = 0, c = 0, d = 0;
    sscanf(strAddr, "%hhu.%hhu.%hhu.%hhu", &a, &b, &c, &d);  
    return ((uint32_t)d<<24) | ((uint32_t)c<<16) | ((uint32_t)b<<8) | ((uint32_t)a);
}