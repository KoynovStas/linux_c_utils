#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>


#include "file_utils.h"





/*
 * des: Set a file descriptor to blocking or non-blocking mode.
 *
 * in:  fd        - The file descriptor
 *      blocking  - 0 set non-blocking mode for fd
 *                - 1 set blocking mode for fd
 *
 * ret:  0 - success
 *      -1 - failure (see errno)
 */
int set_blocking(int fd, int blocking)
{
    // Save the current flags
    int flags = fcntl(fd, F_GETFL, 0);
    if(flags == -1)
        return -1;


    if(blocking)
        flags &= ~O_NONBLOCK;
    else
        flags |= O_NONBLOCK;


    return fcntl(fd, F_SETFL, flags);
}



inline int set_nonblock_mode(int fd)
{
    return set_blocking(fd, 0);
}



inline int set_block_mode(int fd)
{
    return set_blocking(fd, 1);
}
