#ifndef NET_UTILS_HEADER
#define NET_UTILS_HEADER






int get_addr_of_host(const char *host_name, int af, void *addr);
int get_ip_of_host(const char *host_name, int af, char *IP);

int get_addr_of_if(const char *if_name, int af, void *addr);
int get_ip_of_if(const char *if_name, int af, char *IP);


int wait_connect(int sd, unsigned int timeout_in_ms);



#endif //NET_UTILS_HEADER
