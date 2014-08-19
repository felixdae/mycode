#ifndef SOCKET_HELPER_H
#define SOCKET_HELPER_H 

int ConfigureSocket(int ka_time, int ka_interval, int ka_probes, int fd);
int MakeListenSocket(const std::string& ip, uint16_t port);

int32_t SetNonblocking(int sock);
int32_t SetBlocking(int sock);
#endif /* SOCKET_HELPER_H */
