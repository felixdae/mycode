#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include <string>

int32_t SetBlocking(int sock)
{
    int opts = fcntl(sock, F_GETFL);
    assert(opts >= 0);
    if (opts & O_NONBLOCK)
    {
        int rc = fcntl(sock, F_SETFL, opts^O_NONBLOCK);
        assert(rc >= 0);
    }

    struct timeval tv = {1,0};
    int rc = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
    assert(rc >= 0);
    return 0;
}

int32_t SetNonblocking(int sock)
{
    int opts = fcntl(sock, F_GETFL);
    assert(opts >= 0);
    if (!(opts & O_NONBLOCK))
    {
        int rc = fcntl(sock, F_SETFL, opts^O_NONBLOCK);
        assert(rc >= 0);
    }
    return 0;
}

int ConfigureSocket(int ka_time, int ka_interval, int ka_probes, int fd)
{
    int opt_val = 1;
    socklen_t opt_len = sizeof(opt_val);
    if (ka_time) {
        /* enable keepalive on this socket */
        opt_val = 1;
        if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE,
                    (const void *)&opt_val, opt_len) < 0)
            return -1;

        /* set the keepalive conditions we want on it too */
        opt_val = ka_time;
        if (setsockopt(fd, IPPROTO_IP, TCP_KEEPIDLE,
                    (const void *)&opt_val, opt_len) < 0)
            return -1;

        opt_val = ka_interval;
        if (setsockopt(fd, IPPROTO_IP, TCP_KEEPINTVL,
                    (const void *)&opt_val, opt_len) < 0)
            return -1;

        opt_val = ka_probes;
        if (setsockopt(fd, IPPROTO_IP, TCP_KEEPCNT,
                    (const void *)&opt_val, opt_len) < 0)
            return -1;
    }

    /* Disable Nagle */
    opt_val = 1;
    setsockopt(fd, SOL_TCP, TCP_NODELAY, (const void *)&opt_val, opt_len);

    /* We are nonblocking... */
    //fcntl(fd, F_SETFL, O_NONBLOCK);
    SetNonblocking(fd);

//    struct sockaddr_in sin;
//    socklen_t len = sizeof(sin);
//
//    getsockname(fd, (struct sockaddr *)&sin, &len);
//    printf("%s:%d\n", inet_ntoa(sin.sin_addr), ntohs(sin.sin_port));
//    perror("");

   return 0;
}

int MakeListenSocket(const std::string& ip, uint16_t port)
{
    int listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == -1)
    {
        return -1;
    }

    /*
     * allow us to restart even if old sockets in TIME_WAIT
     * (REUSEADDR on Unix means, "don't hang on to this
     * address after the listener is closed."  On Windows, though,
     * it means "don't keep other processes from binding to
     * this address while we're using it)
     */
    int opt = 1;
    setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR,
            (const void *)&opt, sizeof(opt));

    /* Disable Nagle */
    opt = 1;
    setsockopt(listen_sock, IPPROTO_TCP, TCP_NODELAY,
            (const void *)&opt, sizeof(opt));
    //set nonblocking
    //fcntl(listen_sock, F_SETFL, O_NONBLOCK);
    SetNonblocking(listen_sock);

    struct sockaddr_in saddr;
    bzero(&saddr, sizeof(saddr));
    saddr.sin_family = AF_INET;
    saddr.sin_addr.s_addr = inet_addr(ip.c_str());
    saddr.sin_port = htons(port);

    int rc = bind(listen_sock, (struct sockaddr*)&saddr, sizeof(struct sockaddr));
    if (rc == -1)
    {
        return -1;
    }

    rc = listen(listen_sock, SOMAXCONN);
    if (rc == -1)
    {
        return -1;
    }
    return listen_sock;
}

//#include <sys/types.h> 
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netinet/tcp.h>
//#include <arpa/inet.h>
//#include <fcntl.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <errno.h>
//#include <string.h>
//#include <unistd.h>
//#include <assert.h>
//
//int32_t SetBlocking(int sock)
//{
//    int opts = fcntl(sock, F_GETFL);
//    assert(opts >= 0);
//
//    printf("%d, %d\n",__LINE__, (opts & O_NONBLOCK));
//    if (opts & O_NONBLOCK)
//    {
//        int rc = fcntl(sock, F_SETFL, opts^O_NONBLOCK);
//        assert(rc >= 0);
//    }
//    return 0;
//}
//
//int main(int argc, char **argv)
//{
//    struct sockaddr_in saddr;
//    bzero(&saddr, sizeof(saddr));
//    saddr.sin_family = AF_INET;
//    saddr.sin_addr.s_addr = inet_addr(argv[1]);
//    saddr.sin_port = htons(atoi(argv[2]));
//
//    int rc = 0;
//
//    int sock = socket(AF_INET, SOCK_STREAM, 0);
//    if (sock < 0)
//    {
//        perror("socket");
//        return -1;
//    }
//
//    struct timeval tv = {0,0};
//    socklen_t len = sizeof(tv);
//    rc = getsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, &len);
//    if (rc < 0)
//    {
//        perror("getsockopt");
//        return -1;
//    }
//
//    printf("%d, %d\n", tv.tv_sec, tv.tv_usec);
//
//    tv.tv_sec = 30;
//    tv.tv_usec = 0;
////    rc = setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv));
//    if (rc < 0)
//    {
//        perror("setsockopt");
//        return -1;
//    }
//
//    SetBlocking(sock);
//
//    rc = connect(sock, (struct sockaddr*)&saddr, sizeof(struct sockaddr));
//    if (rc < 0)
//    {
//        perror("connect");
//        return -1;
//    }
//
//    printf("%d\n", __LINE__);
//    sleep(30);
//    printf("%d\n", __LINE__);
//
//    char buf[] = "fdsfjksajflksjlkjlkjlkjlkjlkjkljkljafdsfafdasfdsfa\n";
//    rc = send(sock, buf, strlen(buf), MSG_NOSIGNAL);
//    printf("send return: %d\n", rc);
//    if (rc <= 0)
//    {
//        perror("send");
//        return -1;
//    }
//    printf("before return 0\n");
//    return 0;
//}
//
