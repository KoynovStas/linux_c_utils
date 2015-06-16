#include <stdlib.h>
#include <stdio.h>  
#include <unistd.h>  
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>



#include "net_utils.h"








/*
 * des:   get_addr_of_host function returns IP-address in,
 *        given in network byte order for host (host_name).
 *        for converting this address to a string, use the inet_ntop().
 *
 *
 * in:   host_name - the host name in a string format of such "yandex.ru"
 *       af        - Valid address types are AF_INET and AF_INET6
 *       addr      - a pointer to the structure of the address,
 *                   which will be copied to the host address.
 *                   This format is an IP address is necessary
 *                   in functions such as connect()
 *
 * ret:  0 - success
 *      -1 - failure (see errno)
 */
int get_addr_of_host(const char *host_name, int af, void *addr)
{
    struct hostent *host;


    if( !host_name || !addr )
    {
        errno = EINVAL;
        return -1;
    }


    host = gethostbyname2(host_name, af);
    if( !host )
      return -1;                        //possible DNS is not configured


    if( host->h_addrtype != af )
      return -1;                        //addrtype(format) not correct


    if( !host->h_addr_list[0] )
      return -1;                        //host is found, but no associated IP no.


    memcpy(addr, host->h_addr_list[0], host->h_length);


    return 0; //good job
}
