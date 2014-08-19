#ifndef WEBSOCKET_H
#define WEBSOCKET_H 
#include <string>
#include <vector>
#include <map>

#include "socket_helper.h"
#include "frame.h"
#include "deflate_frame.h"

namespace comm
{
    namespace websocket
    {
        class EchoWebSocket
        {
            public:
                EchoWebSocket(int sock, const std::string& remote_ip, uint16_t remote_port,
                        const std::string& wsid);
                virtual ~EchoWebSocket();

                int32_t Read();
                int32_t Write();
                int32_t PlanToWrite(const std::string& text);
                int32_t Ping();
                int32_t Close();
                
                enum class WS_STATE
                {
                    WS_STATE_TCP_CONNECTED,
                    WS_STATE_HANDSHAKE_RECVED,
                    WS_STATE_HANDSHAKE_OK,
                    WS_STATE_SENDING_FRAME,
                    WS_STATE_FRAME_SENT,
                    WS_STATE_RECVING_FRAME,
                    WS_STATE_FRAME_RECVED,
                    WS_STATE_POSITIVE_CLOSING,//i'd like to close
                    WS_STATE_POSITIVE_CLOSED,//close frame sent
                    WS_STATE_PASSIVE_CLOSED,//close frame recved
                    WS_STATE_CLOSED,
                };
                void SetEvents(uint32_t events)
                {
                    this->events_ = events;
                }
                uint32_t GetEvents() const
                {
                    return this->events_;
                }
                WS_STATE GetState() const
                {
                    return this->state_;
                }
                int GetSock() const
                {
                    return this->sock_;
                }
                static std::string GenID(int sock)
                {
                    return std::to_string(conn_count_++);
                }
                std::string GetErrorMsg() const
                {
                    return this->error_msg_;
                }
            private:
                int32_t WriteOneFrame(const std::string& frame);
                class BlockingGuard
                {
                    public:
                        BlockingGuard(int sock)
                        {
                            this->sock_ = sock;
                            SetBlocking(this->sock_);
                        }
                        ~BlockingGuard()
                        {
                            SetNonblocking(this->sock_);
                        }
                    private:
                        int sock_;
                };
            private:
                static uint32_t conn_count_;
                std::string error_msg_;
                WS_STATE state_;
                std::vector<std::string> out_frame_stack_;
                //std::string current_out_frame_;

                uint32_t events_;
                int sock_;
                std::string remote_ip_;
                uint16_t remote_port_;
                std::string wsid_;

                uint32_t send_buf_size_;
                uint32_t recv_buf_size_;
                char *recv_buf_;
            private:
                std::string http_headers_;
                std::map<int32_t, std::string> headers_;
                bool using_deflate_;
                const std::vector<std::string> avail_protocols_;
                const std::vector<std::string> avail_extensions_;
                std::string accept_protocol_;
                std::vector<std::string> accept_extensions_;

                std::string uri_;
                std::string payload_;
                std::shared_ptr<comm::websocket::DeflateFrame> df_;
                std::shared_ptr<comm::websocket::Frame> frame_;
        };
    }
}
#endif /* WEBSOCKET_H */
