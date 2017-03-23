#include "IPV4.h"
#include "checksum.h"

/**
 * @brief Makes the header struct for ipv4
 * @param *header is a pointer to the prealocated header
 * @param saddr is the source ipv4 address
 * @param daddr is the destination ipv4 address
 */
void mkHeader(struct iphdr *header, unsigned int saddr, unsigned int daddr)
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

    (*header).csum = make_csum((void *) &header, IPV4_MIN_LEN, 10);
}

/**
* @brief Packs the header and the data into a package
* @param *header is a pointer to the prealocated header
* @param *data is a pointer to the prealocated data
* @param dataLenght is the lenght of data
* @param *package is a pointer to the prealocated package
*/ 
void makePackage(struct iphdr *header, unsigned char *data, unsigned dataLenght, struct ippkg *package)
{
    (*header).len += dataLenght;

    (*package).header = header;
    (*package).data = data;
}

