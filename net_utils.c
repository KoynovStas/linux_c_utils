#include <stdlib.h>
#include <stdio.h>  
#include <unistd.h>  
#include <string.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>



#include "net_utils.h"
#include "file_utils.h"








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
 * des:   get_ip_of_if function returns IP-address in
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



/*
 * des:   wait_connect function waiting when the complete connect for sd
 *
 *
 * in:   sd             - socket descriptor, for which you spend waiting
 *       timeout_in_ms  - timeout in milliseconds
 *
 * ret:  0 - success
 *      -1 - failure (see errno)
 */
int wait_connect(int sd, unsigned int timeout_in_ms)
{

    struct epoll_event connect_event;
    int epoll_fd, num_events, error;
    socklen_t err_len;



    if( errno != EINPROGRESS )
        return -1;


    epoll_fd = epoll_create(1);
    if( epoll_fd == -1 )
        return -1;



    connect_event.data.fd = sd;
    connect_event.events  = EPOLLOUT | EPOLLIN | EPOLLERR;

    if( epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sd, &connect_event) == -1 )
    {
        close(epoll_fd);
        return -1;
    }


    memset(&connect_event, 0, sizeof(struct epoll_event));
    num_events = epoll_wait(epoll_fd, &connect_event, 1, timeout_in_ms);

    if(num_events <= 0)
    {
        close(epoll_fd);
        return -1;
    }


    error   = -1;
    err_len = sizeof(error);

    if( (getsockopt(sd, SOL_SOCKET, SO_ERROR, &error, &err_len) != 0) || (error != 0) )
    {
        close(epoll_fd);
        return -1;
    }


    close(epoll_fd);
    return 0;         //good job
}



/*
 * des:  connecting to socket for parameters socket_param
 *
 *
 * in:   socket_param  - socket parameters (only IPv4)
 *
 * ret:  != -1 - success (socket descriptor)
 *       == -1 - failure (see errno)
 */
int connect_to_ipv4_socket(struct socket_param_t *socket_param)
{

    struct sockaddr_in  sin;   //for IPv4
    int sd, ret;


    memset(&sin, 0, sizeof(struct sockaddr_in));
    sin.sin_family = AF_INET;
    sin.sin_port   = htons(socket_param->port);


    ret = get_addr_of_host(socket_param->host_or_IP, socket_param->domain, &sin.sin_addr);


    //not host. IP?
    if(ret != 0)
        ret = inet_pton(socket_param->domain, socket_param->host_or_IP, &sin.sin_addr);


    //if not IP, error
    if(ret <= 0)
        return -1;


    sd = socket(socket_param->domain, socket_param->type, socket_param->protocol);
    if( sd == -1 )
        return -1;


    if( socket_param->non_block && set_nonblock_mode(sd) )
    {
        close(sd);
        return -1;  //can't set non_block mode
    }


    if( socket_param->type == SOCK_DGRAM )
        return sd; //good job


    ret = connect(sd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in));
    if( ret &&  wait_connect(sd, 2000) )
    {
        close(sd);
        return -1;  //can't connect to socket
    }


    return sd; //good job
}



/*
 * des:  connecting to socket for parameters socket_param
 *
 *
 * in:   socket_param  - socket parameters (only IPv6)
 *
 * ret:  != -1 - success (socket descriptor)
 *       == -1 - failure (see errno)
 */
int connect_to_ipv6_socket(struct socket_param_t *socket_param)
{

    struct sockaddr_in6  sin;   //for IPv6
    int sd, ret;


    memset(&sin, 0, sizeof(struct sockaddr_in6));
    sin.sin6_family = AF_INET6;
    sin.sin6_port   = htons(socket_param->port);


    ret = get_addr_of_host(socket_param->host_or_IP, socket_param->domain, &sin.sin6_addr);


    //not host. IP?
    if(ret != 0)
        ret = inet_pton(socket_param->domain, socket_param->host_or_IP, &sin.sin6_addr);


    //if not IP, error
    if(ret <= 0)
        return -1;


    sd = socket(socket_param->domain, socket_param->type, socket_param->protocol);
    if( sd == -1 )
        return -1;


    if( socket_param->non_block && set_nonblock_mode(sd) )
    {
        close(sd);
        return -1;  //can't set non_block mode
    }


    if( socket_param->type == SOCK_DGRAM )
        return sd; //good job


    ret = connect(sd, (struct sockaddr *)&sin, sizeof(struct sockaddr_in6));
    if( ret &&  wait_connect(sd, 2000) )
    {
        close(sd);
        return -1;  //can't connect to socket
    }


    return sd; //good job
}



/*
 * des:  connecting to socket for parameters socket_param
 *
 *
 * in:   socket_param  - socket parameters (IPv4 || IPv6)
 *
 * ret:  != -1 - success (socket descriptor)
 *       == -1 - failure (see errno)
 */
int connect_to_socket(struct socket_param_t *socket_param)
{

    if( !socket_param  || !socket_param->host_or_IP )
    {
        errno = EINVAL;
        return -1;
    }


    if(socket_param->domain == AF_INET6)
        return connect_to_ipv6_socket(socket_param);
    else
        return connect_to_ipv4_socket(socket_param);
}
