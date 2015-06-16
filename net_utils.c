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
 *       af        - valid address types are AF_INET and AF_INET6
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



/*
 * des:   get_ip_of_host function returns IP-address in
 *        string format for host (host_name).
 *
 *
 * in:   host_name - the host name in a string format of such "yandex.ru"
 *       af        - valid address types are AF_INET and AF_INET6
 *       IP        - a pointer to the string for IP address
 *
 * ret:  0 - success
 *      -1 - failure (see errno)
 */
int get_ip_of_host(const char *host_name, int af, char *IP)
{
    struct hostent *host;
    int addrstr_len;


    if( !host_name || !IP )
    {
        errno = EINVAL;
        return -1;
    }


    if( af == AF_INET6)
        addrstr_len = INET6_ADDRSTRLEN;
    else
        addrstr_len = INET_ADDRSTRLEN;


    host = gethostbyname2(host_name, af);
    if( !host )
      return -1;                        //possible DNS is not configured


    if( !host->h_addr_list[0] )
      return -1;                        //host is found, but no associated IP no.


    if(inet_ntop(af, (void *)host->h_addr_list[0], IP, addrstr_len) == NULL)
      return -1;


    return 0; //good job
}



/*
 * des:   get_addr_of_if function returns address specified in
 *        the network byte order for the network interface if_name.
 *
 *
 * in:   if_name - network interface name in a string format of such "eth0"
 *       af      - valid address types are AF_INET and AF_INET6
         addr    - a pointer to the structure of the address,
 *                 which will be copied to the host address.
 *                 This format is an IP address is necessary
 *                 in functions such as connect()
 *
 * ret:  0 - success
 *      -1 - failure (see errno)
 */
int get_addr_of_if(const char *if_name, int af, void *addr)
{

    struct ifaddrs *ifa_head;
    struct ifaddrs *ifa_cur;
    int result, addr_len;
    void *src;



    if( !if_name || !addr )
    {
        errno = EINVAL;
        return -1;
    }


    if( getifaddrs(&ifa_head) != 0 )
      return -1;


    result = -1;
    for( ifa_cur = ifa_head;  ifa_cur;  ifa_cur = ifa_cur->ifa_next )
    {

        if( !ifa_cur->ifa_name )
            continue;


        if( !ifa_cur->ifa_addr )
            continue;


        if( ifa_cur->ifa_addr->sa_family != af )
            continue;


        if( strcmp(if_name, (char *)ifa_cur->ifa_name) != 0 )
            continue;



        if( af == AF_INET6)
        {
            addr_len = sizeof(struct sockaddr_in6);
            src      = &(((struct sockaddr_in6 *)ifa_cur->ifa_addr)->sin6_addr);
        }
        else
        {
            addr_len = sizeof(struct sockaddr_in);
            src      = &(((struct sockaddr_in *)ifa_cur->ifa_addr)->sin_addr);
        }


        memcpy(addr, src, addr_len);
        result = 0;  //good job
        break;
    }


    freeifaddrs(ifa_head);

    return result;
}



/*
 * des:   get_ip_of_if function rreturns IP-address in
 *        string format for the network interface if_name.
 *
 *
 * in:   if_name - network interface name in a string format of such "eth0"
 *       af      - valid address types are AF_INET and AF_INET6
 *       IP      - a pointer to the string for IP address
 *
 * ret:  0 - success
 *      -1 - failure (see errno)
 */
int get_ip_of_if(const char *if_name, int af, char *IP)
{

    struct ifaddrs *ifa_head;
    struct ifaddrs *ifa_cur;
    int result, addrstr_len;;
    void *src;



    if( !if_name || !IP )
    {
        errno = EINVAL;
        return -1;
    }


    if( getifaddrs(&ifa_head) != 0 )
      return -1;


    result = -1;
    for( ifa_cur = ifa_head;  ifa_cur;  ifa_cur = ifa_cur->ifa_next )
    {

        if( !ifa_cur->ifa_name )
            continue;


        if( !ifa_cur->ifa_addr )
            continue;


        if( ifa_cur->ifa_addr->sa_family != af )
            continue;


        if( strcmp(if_name, (char *)ifa_cur->ifa_name) != 0 )
            continue;



        if( af == AF_INET6)
        {
            addrstr_len = INET6_ADDRSTRLEN;
            src         = &(((struct sockaddr_in6 *)ifa_cur->ifa_addr)->sin6_addr);
        }
        else
        {
            addrstr_len = INET_ADDRSTRLEN;
            src         = &(((struct sockaddr_in *)ifa_cur->ifa_addr)->sin_addr);
        }


        if(inet_ntop(af, src, IP, addrstr_len) != NULL)
          result = 0;  //good job

        break;
    }


    freeifaddrs(ifa_head);

    return result;
}
