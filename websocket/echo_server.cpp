#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <string>
#include <map>
#include <memory>

#include "echo_websocket.h"
#include "socket_helper.h"
#include "libjson.h"
#include "libdatetime.h"
#include "libprocessutil.h"
#include "init.h"

uint64_t pow2_32 = (uint64_t)1 + (uint32_t)(-1);

int AddNewConnIntoTable(int epfd, int sock, const std::string remote_ip,
        uint16_t remote_port, const struct setting &rs, std::map<int32_t, std::string> &sock2wsid,
        std::map<std::string, std::shared_ptr<comm::websocket::EchoWebSocket>> &wsid2ws)
{
    epoll_event tev;
    tev.events = EPOLLIN;
    tev.data.u64 = ((uint64_t)sock)*(pow2_32) + tev.events;
    int rc = epoll_ctl(epfd, EPOLL_CTL_ADD, sock, &tev);
    if (rc < 0)
    {
        return -1;
    }

    std::string wsid = comm::websocket::EchoWebSocket::GenID(sock);
    std::shared_ptr<comm::websocket::EchoWebSocket> ws(new comm::websocket::EchoWebSocket(sock, remote_ip, remote_port, wsid));
    ws->SetEvents(tev.events);

    wsid2ws[wsid] = ws;
    sock2wsid[sock] = wsid;
    return 0;
}

int RemoveConnFromTable(int epfd, int sock, std::map<int32_t, std::string> &sock2wsid,
        std::map<std::string, std::shared_ptr<comm::websocket::EchoWebSocket>> &wsid2ws)
{
    epoll_ctl(epfd, EPOLL_CTL_DEL, sock, NULL);
    wsid2ws.erase(sock2wsid[sock]);
    sock2wsid.erase(sock);
    return 0;
}

int ModifyConnEvents(int epfd, int sock, std::map<int32_t, std::string> &sock2wsid,
        std::map<std::string, std::shared_ptr<comm::websocket::EchoWebSocket>> &wsid2ws)
{
    if (sock2wsid.count(sock) == 0)
    {
        return 1;
    }
    if (wsid2ws.count(sock2wsid[sock]) == 0)
    {
        return 1;
    }
    epoll_event tev;
    tev.events = wsid2ws[sock2wsid[sock]]->GetEvents();
    tev.data.u64= ((uint64_t)sock)*(pow2_32) + tev.events;
    epoll_ctl(epfd, EPOLL_CTL_MOD, sock, &tev);
    return 0;
}

int main(int argc, char** argv)
{
    struct setting rs;
    LoadSetting(argc, argv, rs);
    comm::library::ProcessUtil::Daemonize();
    CLOG->Init(rs.log_path, rs.log_file_num, rs.log_file_size, rs.verbose, rs.debug);

    std::map<int32_t, std::string> sock2wsid;
    std::map<std::string, std::shared_ptr<comm::websocket::EchoWebSocket>> wsid2ws;
    int listen_sock = MakeListenSocket(rs.listen_ip, rs.listen_port);
    if (listen_sock < 0)
    {
        LOG_NOTICE("MakeListenSocket error");
        return -1;
    }

    //comm::library::ProcessUtil::Fork();

    int epfd = epoll_create1(EPOLL_CLOEXEC);
    if (epfd == -1)
    {
        LOG_NOTICE("epoll_create1 error");
        return -1;
    }

    struct epoll_event listen_event;
    listen_event.events = EPOLLIN;
    listen_event.data.u64 = ((uint64_t)listen_sock)*(pow2_32) + listen_event.events;

    int rc = epoll_ctl(epfd, EPOLL_CTL_ADD, listen_sock, &listen_event);
    if (rc == -1)
    {
        LOG_NOTICE("epoll_ctl error");
        return -1;
    }

    const int max_events = 256;
    struct epoll_event* ret_events = (struct epoll_event*)malloc(max_events * sizeof(struct epoll_event));
    while (!comm::library::ProcessUtil::should_stop_)
    {
        int ret_event_num = epoll_wait(epfd, ret_events, max_events, 1000);
        //LOG_NOTICE("event num: %d", ret_event_num);
        if (ret_event_num < 0)
        {
            if (errno != EINTR)
            {
                break;
            }
            LOG_NOTICE("EINTR");
            continue;
        }

        for (int i = 0; i < ret_event_num; ++i)
        {
            struct epoll_event* ev = ret_events + i;
            int sock = ev->data.u64/(pow2_32);

            if (ev->events & (EPOLLERR|EPOLLHUP)) 
            {
                RemoveConnFromTable(epfd, sock, sock2wsid, wsid2ws);
                close(sock);
                continue;
            }

            if (ev->events & EPOLLIN)
            {
                if (sock == listen_sock)
                {
                    struct sockaddr_in cli_addr;
                    socklen_t clilen = sizeof(cli_addr);
                    int conn = accept(listen_sock, (struct sockaddr *)&cli_addr, &clilen);
                    if (conn < 0)
                    {
                        if (errno == EAGAIN or errno == EWOULDBLOCK)
                        {
                            LOG_NOTICE("accept try again?");
                            continue;
                        }
                        LOG_NOTICE("accept error: %s", strerror(errno));
                        comm::library::ProcessUtil::should_stop_ = true;
                        //stop = 1;
                        break;
                    }
                    char remote_ip[20];
                    strncpy(remote_ip, inet_ntoa(cli_addr.sin_addr), sizeof(remote_ip));
                    LOG_NOTICE("new conn: %d from %s:%d", conn, remote_ip, ntohs(cli_addr.sin_port));
                    ConfigureSocket(0, 0, 0, conn);

                    int rc = AddNewConnIntoTable(epfd, conn, remote_ip, ntohs(cli_addr.sin_port),
                            rs, sock2wsid, wsid2ws);
                    if (rc < 0)
                    {
                        close(conn);
                    }
                }
                else
                {
                    if (sock2wsid.count(sock) == 0)
                    {
                        epoll_ctl(epfd, EPOLL_CTL_DEL, sock, NULL);
                        close(sock);
                    }
                    std::shared_ptr<comm::websocket::EchoWebSocket> ws = wsid2ws[sock2wsid[sock]];
                    int rc = ws->Read();
                    if (rc < 0)
                    {
                        RemoveConnFromTable(epfd, sock, sock2wsid, wsid2ws);
                        close(sock);
                    }
                }
            }

            if(ev->events & EPOLLOUT)
            {
                if (sock2wsid.count(sock) == 0)
                {
                    epoll_ctl(epfd, EPOLL_CTL_DEL, sock, NULL);
                    close(sock);
                }
                else
                {
                    std::shared_ptr<comm::websocket::EchoWebSocket> ws = wsid2ws[sock2wsid[sock]];
                    int rc = ws->Write();
                    if (rc < 0)
                    {
                        RemoveConnFromTable(epfd, sock, sock2wsid, wsid2ws);
                        close(sock);
                    }
                }
            }
        }
        for(auto pr:sock2wsid)
        {
            ModifyConnEvents(epfd, pr.first, sock2wsid, wsid2ws);
        }
    }
    LOG_NOTICE("exit normally");
    free(ret_events);
    close(epfd);
    return 0;
}

