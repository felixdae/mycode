#ifndef FRAME_H
#define FRAME_H 
#include <string>
#include <memory>

#include "deflate_frame.h"

namespace comm
{
    namespace websocket
    {
        class Frame
        {
            private:
                static int32_t GenerateMask(uint8_t nonce_04[]);
            public:
                explicit Frame(std::shared_ptr<comm::websocket::DeflateFrame> df);
                virtual ~Frame();
                bool Prepare4Parse();
                enum libwebsocket_write_protocol
                {
                    LWS_WRITE_TEXT,
                    LWS_WRITE_BINARY,
                    LWS_WRITE_CONTINUATION,
                    LWS_WRITE_HTTP,

                    /* special 04+ opcodes */

                    LWS_WRITE_CLOSE,
                    LWS_WRITE_PING,
                    LWS_WRITE_PONG,

                    /* flags */

                    LWS_WRITE_NO_FIN = 0x40,
                    /*
                     * client packet payload goes out on wire unmunged
                     * only useful for security tests since normal servers cannot
                     * decode the content if used
                     */
                    LWS_WRITE_CLIENT_IGNORE_XOR_MASK = 0x80
                };

                enum lws_close_status 
                {
                    LWS_CLOSE_STATUS_NOSTATUS = 0,
                    LWS_CLOSE_STATUS_NORMAL = 1000,
                    LWS_CLOSE_STATUS_GOINGAWAY = 1001,
                    LWS_CLOSE_STATUS_PROTOCOL_ERR = 1002,
                    LWS_CLOSE_STATUS_UNACCEPTABLE_OPCODE = 1003,
                    LWS_CLOSE_STATUS_RESERVED = 1004,
                    LWS_CLOSE_STATUS_NO_STATUS = 1005,
                    LWS_CLOSE_STATUS_ABNORMAL_CLOSE = 1006,
                    LWS_CLOSE_STATUS_INVALID_PAYLOAD = 1007,
                    LWS_CLOSE_STATUS_POLICY_VIOLATION = 1008,
                    LWS_CLOSE_STATUS_MESSAGE_TOO_LARGE = 1009,
                    LWS_CLOSE_STATUS_EXTENSION_REQUIRED = 1010,
                    LWS_CLOSE_STATUS_UNEXPECTED_CONDITION = 1011,
                    LWS_CLOSE_STATUS_TLS_FAILURE = 1015,
                };


            private:
                struct _lws_websocket_related {
                    char *rx_user_buffer;
                    int rx_user_buffer_head;
                    unsigned char frame_masking_nonce_04[4];
                    unsigned char frame_mask_index;
                    size_t rx_packet_length;
                    unsigned char opcode;
                    unsigned int fin:1;
                    unsigned char rsv;
                    unsigned int frame_is_binary:1;
                    unsigned int all_zero_nonce:1;
                    short close_reason; /* enum lws_close_status */
                    unsigned char *rxflow_buffer;
                    int rxflow_len;
                    int rxflow_pos;
                    unsigned int rxflow_change_to:2;
                    unsigned int this_frame_masked:1;
                    unsigned int inside_frame:1; /* next write will be more of frame */
                    unsigned int clean_buffer:1; /* buffer not rewritten by extension */
                    /* truncated send handling */
                    unsigned char *truncated_send_malloc; /* non-NULL means buffering in progress */
                    unsigned int truncated_send_offset; /* where we are in terms of spilling */
                    unsigned int truncated_send_len; /* how much is buffered */
                };
            private:
                enum lws_websocket_opcodes_07 
                {
                    LWS_WS_OPCODE_07__CONTINUATION = 0,
                    LWS_WS_OPCODE_07__TEXT_FRAME = 1,
                    LWS_WS_OPCODE_07__BINARY_FRAME = 2,

                    LWS_WS_OPCODE_07__NOSPEC__MUX = 7,

                    /* control extensions 8+ */

                    LWS_WS_OPCODE_07__CLOSE = 8,
                    LWS_WS_OPCODE_07__PING = 9,
                    LWS_WS_OPCODE_07__PONG = 0xa,
                };
                enum lws_rx_parse_state {
                    LWS_RXPS_NEW,

                    LWS_RXPS_04_MASK_NONCE_1,
                    LWS_RXPS_04_MASK_NONCE_2,
                    LWS_RXPS_04_MASK_NONCE_3,

                    LWS_RXPS_04_FRAME_HDR_1,
                    LWS_RXPS_04_FRAME_HDR_LEN,
                    LWS_RXPS_04_FRAME_HDR_LEN16_2,
                    LWS_RXPS_04_FRAME_HDR_LEN16_1,
                    LWS_RXPS_04_FRAME_HDR_LEN64_8,
                    LWS_RXPS_04_FRAME_HDR_LEN64_7,
                    LWS_RXPS_04_FRAME_HDR_LEN64_6,
                    LWS_RXPS_04_FRAME_HDR_LEN64_5,
                    LWS_RXPS_04_FRAME_HDR_LEN64_4,
                    LWS_RXPS_04_FRAME_HDR_LEN64_3,
                    LWS_RXPS_04_FRAME_HDR_LEN64_2,
                    LWS_RXPS_04_FRAME_HDR_LEN64_1,

                    LWS_RXPS_07_COLLECT_FRAME_KEY_1,
                    LWS_RXPS_07_COLLECT_FRAME_KEY_2,
                    LWS_RXPS_07_COLLECT_FRAME_KEY_3,
                    LWS_RXPS_07_COLLECT_FRAME_KEY_4,

                    LWS_RXPS_PAYLOAD_UNTIL_LENGTH_EXHAUSTED
                };

                int libwebsocket_rx_sm(struct _lws_websocket_related *ws, unsigned char c);
                int lws_rx_parse_state;
                std::string rx_user_buffer_;
                std::string recv_data_;
                uint32_t pos_;
                struct _lws_websocket_related ws_;

                std::string error_msg_;
                std::shared_ptr<comm::websocket::DeflateFrame> df_;
            public:
                std::string GetErrorMsg() const
                {
                    return this->error_msg_;
                }

                struct FrameInfo
                {
                    enum libwebsocket_write_protocol proto;
                    char reserved;
                    char fin;
                    std::string payload;
                };
                std::string MakeFrame(const std::string& in, enum libwebsocket_write_protocol protocol, 
                        bool do_mask, bool deflate_frame, enum lws_close_status close_reason);
                int32_t AppendRecvData(const std::string& in);
                int32_t ParseFrame(FrameInfo &fr, std::string& rest);
        };
    }
}
#endif /* FRAME_H */
