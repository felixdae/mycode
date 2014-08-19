#include <sys/epoll.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <algorithm>
#include <exception>
#include <assert.h>

#include "echo_websocket.h"
#include "server_handshake.h"
#include "deflate_frame.h"
#include "init.h"

#include "libcrypto.h"
#include "libdatetime.h"

namespace comm
{
    namespace websocket
    {
        uint32_t EchoWebSocket::conn_count_ = 0;
        EchoWebSocket::EchoWebSocket(int sock, const std::string& remote_ip, uint16_t remote_port,
                const std::string& wsid)
        :state_(WS_STATE::WS_STATE_TCP_CONNECTED),
        events_(0), sock_(sock), remote_ip_(remote_ip), remote_port_(remote_port),
        wsid_(wsid), send_buf_size_(0), recv_buf_size_(0), recv_buf_(NULL),
        using_deflate_(false), avail_protocols_({"default-protocol"}),
        avail_extensions_({"deflate-frame", "x-webkit-deflate-frame"}), 
        uri_("/")
        {
            int v = 0;
            socklen_t l = sizeof(v);
            int rc = getsockopt(this->sock_, SOL_SOCKET, SO_SNDBUF, &v, &l);
            assert(rc == 0);
            this->send_buf_size_ = v;
            v = 0;
            rc = getsockopt(this->sock_, SOL_SOCKET, SO_RCVBUF, &v, &l);
            assert(rc == 0);
            this->recv_buf_size_ = v;//getsockopt
            this->recv_buf_ = (char*)malloc(this->recv_buf_size_);

            this->df_ = std::shared_ptr<comm::websocket::DeflateFrame>(new comm::websocket::DeflateFrame(true));
            this->frame_ = std::shared_ptr<comm::websocket::Frame>(new comm::websocket::Frame(this->df_));
            this->frame_->Prepare4Parse();
        }

        EchoWebSocket::~EchoWebSocket()
        {
            if (this->recv_buf_ != NULL)
            {
                free(this->recv_buf_);
            }
        }

        int32_t EchoWebSocket::Read()
        {
            if (this->state_ == WS_STATE::WS_STATE_CLOSED or this->state_ == WS_STATE::WS_STATE_PASSIVE_CLOSED)
            {
                LOG_NOTICE("closing or closed");
                return -1;
            }

            int n = recv(this->sock_, this->recv_buf_, this->recv_buf_size_, 0);
            if (n == 0)
            {
                LOG_NOTICE("recv 0 byte, client closed");
                return -1;
            }
            if(n < 0)
            {
                if (errno == EAGAIN or errno == EWOULDBLOCK)
                {
                    LOG_NOTICE("recv try again?");
                    return 0;
                }
                else
                {
                    char err_buf[50];
                    LOG_NOTICE("recv error: %s", strerror_r(errno, err_buf, sizeof(err_buf)));
                    return -1;
                }
            }

            if (this->state_ == WS_STATE::WS_STATE_TCP_CONNECTED)
            {
                this->http_headers_ += std::string(this->recv_buf_, n);
                if (this->http_headers_.length() < 4)
                {
                    return 0;
                }
                if (this->http_headers_.length() > 100000)
                {
                    LOG_NOTICE("http headers too long: %u", this->http_headers_.length());
                    return -1;
                }
                if (this->http_headers_.substr(this->http_headers_.length() - 4) == "\r\n\r\n")
                {
                    //parse handshake
                    comm::websocket::ServerHandshake hs;
                    int rc = hs.ParseRequest(this->http_headers_, this->headers_);
                    if (rc != 0)
                    {
                        return -1;
                    }
                    LOG_NOTICE("recive handshake");
                    this->accept_protocol_ = comm::websocket::ServerHandshake::NegotiateProtocol(this->avail_protocols_, this->headers_); 
                    this->accept_extensions_ = comm::websocket::ServerHandshake::NegotiateExtension(this->avail_extensions_, this->headers_);
                    if (std::find(this->accept_extensions_.begin(), this->accept_extensions_.end(), "deflate-frame") != this->accept_extensions_.end()
                            or std::find(this->accept_extensions_.begin(), this->accept_extensions_.end(), "x-webkit-deflate-frame") != this->accept_extensions_.end())
                    {
                        this->using_deflate_ = true;
                    }
                    std::string resp;
                    //todo ping pong close
                    rc = hs.MakeResponse(this->headers_, this->accept_protocol_, this->accept_extensions_, this->uri_, resp);
                    if (rc != 0)
                    {
                        return -1;
                    }
                    this->out_frame_stack_.push_back(resp);
                    this->events_ = EPOLLIN|EPOLLOUT;
                    this->state_ = WS_STATE::WS_STATE_HANDSHAKE_RECVED;
                    this->http_headers_.clear();
                    return 0;
                }
                else
                {
                    if (this->http_headers_.find("\r\n\r\n") == std::string::npos)
                    {
                        //continue reading
                        return 0;
                    }
                    else
                    {
                        LOG_NOTICE("illegal handshake: %s", this->http_headers_.c_str());
                        return -1;
                    }
                }
            }
            else
            {
                this->frame_->AppendRecvData(std::string(this->recv_buf_, n));
                std::string rest;
                comm::websocket::Frame::FrameInfo fi;
                int rc = this->frame_->ParseFrame(fi, rest);
                if (rc == 0)
                {
                    this->frame_->Prepare4Parse();
                    this->frame_->AppendRecvData(rest);

                    if (fi.proto == comm::websocket::Frame::LWS_WRITE_TEXT
                            or fi.proto == comm::websocket::Frame::LWS_WRITE_BINARY
                            or fi.proto == comm::websocket::Frame::LWS_WRITE_CONTINUATION)
                    {
                        //std::string payload = fi.payload;
                        if (this->using_deflate_)
                        {
                            assert((fi.reserved & 0x40) != 0);
                            //comm::websocket::DeflateFrame df(true);
                            try
                            {
                                this->payload_ += this->df_->Inflate(fi.payload);
                            }
                            catch (std::exception &ex)
                            {
                                LOG_NOTICE("inflate exception: %s", ex.what());
                                return -1;
                            }
                        }
                        else
                        {
                            this->payload_ += fi.payload;
                        }
                        if (this->payload_.length() > 1000000)
                        {
                            LOG_NOTICE("frame too long: %u", this->payload_.length());
                            return -1;
                        }
                        LOG_NOTICE("recv %s ws frame: %s", (fi.fin? "final" : ""), this->payload_.c_str());
                        if (fi.fin)
                        {
                            FUNC_GUARD();
                            this->PlanToWrite(this->payload_);
                            //std::string msg = "{\"wsid\":\"" + this->wsid_ + "\",\"wsmsg\":\"" + comm::library::Crypto::B64encode(this->payload_) + "\"}";
                            //this->wq_->LPush(this->tube_, msg);
                            this->payload_.clear();
                        }
                        this->state_ = WS_STATE::WS_STATE_FRAME_RECVED;
                        return 0;
                    }
                    else if(fi.proto == comm::websocket::Frame::LWS_WRITE_PING)
                    {
                        std::string resp = this->frame_->MakeFrame("", 
                                comm::websocket::Frame::LWS_WRITE_PONG, false, 
                                this->using_deflate_, comm::websocket::Frame::LWS_CLOSE_STATUS_NOSTATUS);
                        this->out_frame_stack_.push_back(resp);
                        this->events_ = EPOLLIN|EPOLLOUT;
                    }
                    else if(fi.proto == comm::websocket::Frame::LWS_WRITE_PONG)
                    {
                        return 0;
                    }
                    else if(fi.proto == comm::websocket::Frame::LWS_WRITE_CLOSE)
                    {
                        if (this->state_ == WS_STATE::WS_STATE_POSITIVE_CLOSED)
                        {
                            this->state_ = WS_STATE::WS_STATE_CLOSED;
                            return -1;
                        }
                        this->state_ = WS_STATE::WS_STATE_PASSIVE_CLOSED;
                        std::string resp = this->frame_->MakeFrame("", 
                                comm::websocket::Frame::LWS_WRITE_CLOSE, false, 
                                this->using_deflate_, comm::websocket::Frame::LWS_CLOSE_STATUS_NOSTATUS);
                        this->out_frame_stack_.push_back(resp);
                        this->events_ = EPOLLIN|EPOLLOUT;
                    }
                    else
                    {
                        return -1;
                    }
                }
                else if (rc < 0)
                {
                    return -1;
                }
                else
                {
                    this->state_ = WS_STATE::WS_STATE_RECVING_FRAME;
                    return 0;
                }
            }
            return -1;
        }

        int32_t EchoWebSocket::PlanToWrite(const std::string& text)
        {
            if (text.empty())
            {
                return 1;
            }
            std::string resp = this->frame_->MakeFrame(text, 
                    comm::websocket::Frame::LWS_WRITE_TEXT, false, 
                    this->using_deflate_,
                    comm::websocket::Frame::LWS_CLOSE_STATUS_NOSTATUS);
            this->out_frame_stack_.push_back(resp);
            this->events_ = EPOLLIN|EPOLLOUT;
            return 0;
        }

                
        int32_t EchoWebSocket::WriteOneFrame(const std::string& frame)
        {
            if (frame.empty())
            {
//                this->state_ = WS_STATE::WS_STATE_FRAME_SENT;
//                this->events_ = EPOLLIN;
                return 0;
            }
            assert(frame.length() <= this->send_buf_size_);
            int n = send(this->sock_, frame.c_str(), frame.length(), MSG_NOSIGNAL);
            if (n == 0)
            {
                LOG_NOTICE("send 0 byte?");
                return -1;
            }
            if(n < 0)
            {
                char err_buf[50];
                LOG_NOTICE("send error: %s", strerror_r(errno, err_buf, sizeof(err_buf)));
                return -1;
            }

            assert((uint32_t)n == frame.length());
            return 0;
        }

        int32_t EchoWebSocket::Write()
        {
            BlockingGuard bg(this->sock_);
            if (this->state_ == WS_STATE::WS_STATE_CLOSED or this->state_ == WS_STATE::WS_STATE_POSITIVE_CLOSED)
            {
                return -1;
            }
            for(auto it = this->out_frame_stack_.begin(); it != this->out_frame_stack_.end(); ++it)
            {
                if (this->WriteOneFrame(*it) != 0)
                {
                    return -1;
                }
            }

            this->out_frame_stack_.clear();
            if (this->state_ == WS_STATE::WS_STATE_HANDSHAKE_RECVED)
            {
                this->state_ = WS_STATE::WS_STATE_HANDSHAKE_OK;
                this->events_ = EPOLLIN;
                return 0;
            }
            if (this->state_ == WS_STATE::WS_STATE_POSITIVE_CLOSING)
            {
                this->state_ = WS_STATE::WS_STATE_FRAME_SENT;
                this->events_ = EPOLLIN;
                return 0;
            }
            if (this->state_ == WS_STATE::WS_STATE_PASSIVE_CLOSED)
            {
                this->state_ = WS_STATE::WS_STATE_CLOSED;
                return -1;
            }
            this->state_ = WS_STATE::WS_STATE_FRAME_SENT;
            this->events_ = EPOLLIN;
            return 0;
        }

        int32_t EchoWebSocket::Ping()
        {
            std::string resp = this->frame_->MakeFrame("", 
                    comm::websocket::Frame::LWS_WRITE_PING, false, 
                    this->using_deflate_,
                    comm::websocket::Frame::LWS_CLOSE_STATUS_NOSTATUS);
            this->out_frame_stack_.push_back(resp);
            this->state_ = WS_STATE::WS_STATE_SENDING_FRAME;
            this->events_ = EPOLLIN|EPOLLOUT;
            return 0;
        }

        int32_t EchoWebSocket::Close()
        {
            std::string resp = this->frame_->MakeFrame("", 
                    comm::websocket::Frame::LWS_WRITE_CLOSE, false, 
                    this->using_deflate_,
                    comm::websocket::Frame::LWS_CLOSE_STATUS_NOSTATUS);
            this->out_frame_stack_.push_back(resp);
            this->state_ = WS_STATE::WS_STATE_POSITIVE_CLOSING;
            this->events_ = EPOLLIN|EPOLLOUT;
            return 0;
        }
    }
}

//gcc -I ../../comm/library/ deflate_frame.cpp frame.cpp server.cpp server_handshake.cpp socket_helper.cpp websocket.cpp ../../comm/library/libstringhelper.cpp ../../comm/library/libcrypto.cpp -std=c++11 -g -Wall -lmcrypt -lcrypto -lstdc++ -lz

