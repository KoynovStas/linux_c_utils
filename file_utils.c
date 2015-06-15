#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <dirent.h>

#include <sys/types.h>


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



/*
 * des:  function returns the number of files in a directory
 *
 * in:   const char *dir_name - the full path to the directory
 *
 * ret:  >= 0 - valid value
 *         -1 - failure (see errno)
 */
int count_files_in_dir(const char *dir_name)
{

    DIR *dir;
    struct dirent *entry;
    int count = 0;


    if( !dir_name )
    {
        errno = EINVAL;
        return -1;
    }


    dir = opendir(dir_name);
    if(dir == NULL)
        return -1;


    while( (entry = readdir(dir)) )
        count++;


    closedir(dir);
    return count;
}
