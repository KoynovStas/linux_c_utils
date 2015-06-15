#ifndef FILE_UTILS_HEADER
#define FILE_UTILS_HEADER





int set_blocking(int fd, int blocking);

int set_nonblock_mode(int fd);
int set_block_mode(int fd);


int count_files_in_dir(const char *dir_name);





#endif //FILE_UTILS_HEADER
