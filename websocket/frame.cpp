#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include <assert.h>
#include <string.h>

#include <memory>

#include "frame.h"

//static char* tmp;
namespace comm
{
    namespace websocket
    {
        Frame::Frame(std::shared_ptr<comm::websocket::DeflateFrame> df)
        :df_(df)
        {
        }

        Frame::~Frame()
        {
        }

        int32_t Frame::GenerateMask(uint8_t nonce_04[])
        {
            int fd_random = open("/dev/urandom", O_RDONLY);
            int n = read(fd_random, nonce_04, 4);;

            //int n = 4;
            //for (int i = 0; i < 8; i += 2)
            //{
            //    nonce_04[i/2] = std::stoi(std::string(tmp + i, 2), NULL, 16);
            //}
            return n;
        }

        std::string Frame::MakeFrame(const std::string& in, enum libwebsocket_write_protocol protocol, 
                bool do_mask, bool deflate_frame, enum lws_close_status close_reason)
        {
            if (protocol == LWS_WRITE_HTTP)
            {
                return in;
            }

            if (in.empty())
            {
                assert(protocol == LWS_WRITE_CLOSE or protocol == LWS_WRITE_PING or protocol == LWS_WRITE_PONG);
            }

            std::shared_ptr<uint8_t> sh_buf(new uint8_t[in.length()], [](uint8_t* p){delete[] p;});
            memcpy(sh_buf.get(), in.c_str(), in.length());
            uint8_t* buf = sh_buf.get();
            size_t len = in.length();

            bool rsv = false;
            if (!in.empty())
            {
                if(deflate_frame)
                {
                    //comm::websocket::DeflateFrame df(true);
                    std::string out = this->df_->Deflate(in);
                    std::shared_ptr<uint8_t> t(new uint8_t[out.length()], [](uint8_t* p){delete[] p;});
                    sh_buf.swap(t);
                    memcpy(sh_buf.get(), out.c_str(), out.length());
                    buf = sh_buf.get();
                    len = out.length();
                    rsv = true;
                }
            }
            else if ((protocol&0xf) == LWS_WRITE_CLOSE)
            {
                uint16_t cr = close_reason;
                cr = htons(cr);
                buf = (uint8_t*)&cr;
                len = 2;
            }

            uint8_t prefix[32];
            uint8_t *p = prefix + sizeof(prefix);
            int pre = 0;
            int ietf_spec_revision = 13;

            assert(ietf_spec_revision == 13);
            uint8_t is_masked_bit = 0;
            if (do_mask and len > 0)
            {
                pre += 4;
                is_masked_bit = 0x80;
                uint8_t nonce_04[4];
                Frame::GenerateMask(nonce_04);
                uint8_t mask_index = 0;
                /*
                 * in v7, just mask the payload
                 */
                for (int i = 0; i < (int)len; i++)
                    buf[i] = buf[i] ^ nonce_04[(mask_index++) & 3];

                memcpy(&p[-pre], nonce_04, 4);
            }

            int n = 0;
            switch (protocol & 0xf)
            {
                case LWS_WRITE_PING:
                    n = LWS_WS_OPCODE_07__PING;
                case LWS_WRITE_PONG:
                    n = LWS_WS_OPCODE_07__PONG;
                case LWS_WRITE_CLOSE:
                    n = LWS_WS_OPCODE_07__CLOSE;
                    break;
                case LWS_WRITE_TEXT:
                    n = LWS_WS_OPCODE_07__TEXT_FRAME;
                    break;
                case LWS_WRITE_BINARY:
                    n = LWS_WS_OPCODE_07__BINARY_FRAME;
                    break;
                case LWS_WRITE_CONTINUATION:
                    n = LWS_WS_OPCODE_07__CONTINUATION;
                    break;
                default:
                    assert(0);
            }

            if (!(protocol & LWS_WRITE_NO_FIN))
                n |= 1 << 7;

            if (len < 126) 
            {
                pre += 2;
                p[-pre] = n;
                p[-pre + 1] = len | is_masked_bit;
            }
            else 
            {
                if (len < 65536) 
                {
                    pre += 4;
                    p[-pre] = n;
                    p[-pre + 1] = 126 | is_masked_bit;
                    p[-pre + 2] = len >> 8;
                    p[-pre + 3] = len;
                }
                else 
                {
                    pre += 10;
                    p[-pre] = n;
                    p[-pre + 1] = 127 | is_masked_bit;
#if defined __LP64__
                    p[-pre + 2] = (len >> 56) & 0x7f;
                    p[-pre + 3] = len >> 48;
                    p[-pre + 4] = len >> 40;
                    p[-pre + 5] = len >> 32;
#else
                    p[-pre + 2] = 0;
                    p[-pre + 3] = 0;
                    p[-pre + 4] = 0;
                    p[-pre + 5] = 0;
#endif
                    p[-pre + 6] = len >> 24;
                    p[-pre + 7] = len >> 16;
                    p[-pre + 8] = len >> 8;
                    p[-pre + 9] = len;
                }
            }
            if (rsv)
            {
                p[-pre] |= 0x40;
            }
            return std::string((char*)(p - pre), pre) + std::string((char*)buf, len);
        }

        int Frame::libwebsocket_rx_sm(struct _lws_websocket_related *ws, unsigned char c)
        {
            int ietf_spec_revision = 13;
            switch (this->lws_rx_parse_state) {
                case LWS_RXPS_NEW:

                    switch (ietf_spec_revision) {
                        case 13:
                            /*
                             * no prepended frame key any more
                             */
                            ws->all_zero_nonce = 1;
                            goto handle_first;

                        default:
                            //lwsl_warn("lws_rx_sm: unknown spec version %d\n",
                            //        ietf_spec_revision);
                            break;
                    }
                    break;
                case LWS_RXPS_04_MASK_NONCE_1:
                    ws->frame_masking_nonce_04[1] = c;
                    if (c)
                        ws->all_zero_nonce = 0;
                    this->lws_rx_parse_state = LWS_RXPS_04_MASK_NONCE_2;
                    break;
                case LWS_RXPS_04_MASK_NONCE_2:
                    ws->frame_masking_nonce_04[2] = c;
                    if (c)
                        ws->all_zero_nonce = 0;
                    this->lws_rx_parse_state = LWS_RXPS_04_MASK_NONCE_3;
                    break;
                case LWS_RXPS_04_MASK_NONCE_3:
                    ws->frame_masking_nonce_04[3] = c;
                    if (c)
                        ws->all_zero_nonce = 0;

                    /*
                     * start from the zero'th byte in the XOR key buffer since
                     * this is the start of a frame with a new key
                     */

                    ws->frame_mask_index = 0;

                    this->lws_rx_parse_state = LWS_RXPS_04_FRAME_HDR_1;
                    break;

                    /*
                     *  04 logical framing from the spec (all this is masked when incoming
                     *  and has to be unmasked)
                     *
                     * We ignore the possibility of extension data because we don't
                     * negotiate any extensions at the moment.
                     *
                     *    0                   1                   2                   3
                     *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
                     *   +-+-+-+-+-------+-+-------------+-------------------------------+
                     *   |F|R|R|R| opcode|R| Payload len |    Extended payload length    |
                     *   |I|S|S|S|  (4)  |S|     (7)     |             (16/63)           |
                     *   |N|V|V|V|       |V|             |   (if payload len==126/127)   |
                     *   | |1|2|3|       |4|             |                               |
                     *   +-+-+-+-+-------+-+-------------+ - - - - - - - - - - - - - - - +
                     *   |     Extended payload length continued, if payload len == 127  |
                     *   + - - - - - - - - - - - - - - - +-------------------------------+
                     *   |                               |         Extension data        |
                     *   +-------------------------------+ - - - - - - - - - - - - - - - +
                     *   :                                                               :
                     *   +---------------------------------------------------------------+
                     *   :                       Application data                        :
                     *   +---------------------------------------------------------------+
                     *
                     *  We pass payload through to userland as soon as we get it, ignoring
                     *  FIN.  It's up to userland to buffer it up if it wants to see a
                     *  whole unfragmented block of the original size (which may be up to
                     *  2^63 long!)
                     */

                case LWS_RXPS_04_FRAME_HDR_1:
handle_first:

                    ws->opcode = c & 0xf;
                    ws->rsv = c & 0x70;
                    ws->fin = !!((c >> 7) & 1);

                    switch (ws->opcode) {
                        case LWS_WS_OPCODE_07__TEXT_FRAME:
                        case LWS_WS_OPCODE_07__BINARY_FRAME:
                            ws->frame_is_binary =
                                ws->opcode == LWS_WS_OPCODE_07__BINARY_FRAME;
                            break;
                    }
                    this->lws_rx_parse_state = LWS_RXPS_04_FRAME_HDR_LEN;
                    break;

                case LWS_RXPS_04_FRAME_HDR_LEN:

                    ws->this_frame_masked = !!(c & 0x80);

                    switch (c & 0x7f) {
                        case 126:
                            /* control frames are not allowed to have big lengths */
                            if (ws->opcode & 8)
                                goto illegal_ctl_length;

                            this->lws_rx_parse_state = LWS_RXPS_04_FRAME_HDR_LEN16_2;
                            break;
                        case 127:
                            /* control frames are not allowed to have big lengths */
                            if (ws->opcode & 8)
                                goto illegal_ctl_length;

                            this->lws_rx_parse_state = LWS_RXPS_04_FRAME_HDR_LEN64_8;
                            break;
                        default:
                            ws->rx_packet_length = c & 0x7f;
                            if (ws->this_frame_masked)
                                this->lws_rx_parse_state =
                                    LWS_RXPS_07_COLLECT_FRAME_KEY_1;
                            else
                                this->lws_rx_parse_state =
                                    LWS_RXPS_PAYLOAD_UNTIL_LENGTH_EXHAUSTED;
                            break;
                    }
                    break;

                case LWS_RXPS_04_FRAME_HDR_LEN16_2:
                    ws->rx_packet_length = c << 8;
                    this->lws_rx_parse_state = LWS_RXPS_04_FRAME_HDR_LEN16_1;
                    break;

                case LWS_RXPS_04_FRAME_HDR_LEN16_1:
                    ws->rx_packet_length |= c;
                    if (ws->this_frame_masked)
                        this->lws_rx_parse_state =
                            LWS_RXPS_07_COLLECT_FRAME_KEY_1;
                    else
                        this->lws_rx_parse_state =
                            LWS_RXPS_PAYLOAD_UNTIL_LENGTH_EXHAUSTED;
                    break;

                case LWS_RXPS_04_FRAME_HDR_LEN64_8:
                    if (c & 0x80) {
                        //lwsl_warn("b63 of length must be zero\n");
                        /* kill the connection */
                        return -1;
                    }
#if defined __LP64__
                    ws->rx_packet_length = ((size_t)c) << 56;
#else
                    ws->rx_packet_length = 0;
#endif
                    this->lws_rx_parse_state = LWS_RXPS_04_FRAME_HDR_LEN64_7;
                    break;

                case LWS_RXPS_04_FRAME_HDR_LEN64_7:
#if defined __LP64__
                    ws->rx_packet_length |= ((size_t)c) << 48;
#endif
                    this->lws_rx_parse_state = LWS_RXPS_04_FRAME_HDR_LEN64_6;
                    break;

                case LWS_RXPS_04_FRAME_HDR_LEN64_6:
#if defined __LP64__
                    ws->rx_packet_length |= ((size_t)c) << 40;
#endif
                    this->lws_rx_parse_state = LWS_RXPS_04_FRAME_HDR_LEN64_5;
                    break;

                case LWS_RXPS_04_FRAME_HDR_LEN64_5:
#if defined __LP64__
                    ws->rx_packet_length |= ((size_t)c) << 32;
#endif
                    this->lws_rx_parse_state = LWS_RXPS_04_FRAME_HDR_LEN64_4;
                    break;

                case LWS_RXPS_04_FRAME_HDR_LEN64_4:
                    ws->rx_packet_length |= ((size_t)c) << 24;
                    this->lws_rx_parse_state = LWS_RXPS_04_FRAME_HDR_LEN64_3;
                    break;

                case LWS_RXPS_04_FRAME_HDR_LEN64_3:
                    ws->rx_packet_length |= ((size_t)c) << 16;
                    this->lws_rx_parse_state = LWS_RXPS_04_FRAME_HDR_LEN64_2;
                    break;

                case LWS_RXPS_04_FRAME_HDR_LEN64_2:
                    ws->rx_packet_length |= ((size_t)c) << 8;
                    this->lws_rx_parse_state = LWS_RXPS_04_FRAME_HDR_LEN64_1;
                    break;

                case LWS_RXPS_04_FRAME_HDR_LEN64_1:
                    ws->rx_packet_length |= ((size_t)c);
                    if (ws->this_frame_masked)
                        this->lws_rx_parse_state =
                            LWS_RXPS_07_COLLECT_FRAME_KEY_1;
                    else
                        this->lws_rx_parse_state =
                            LWS_RXPS_PAYLOAD_UNTIL_LENGTH_EXHAUSTED;
                    break;

                case LWS_RXPS_07_COLLECT_FRAME_KEY_1:
                    ws->frame_masking_nonce_04[0] = c;
                    if (c)
                        ws->all_zero_nonce = 0;
                    this->lws_rx_parse_state = LWS_RXPS_07_COLLECT_FRAME_KEY_2;
                    break;

                case LWS_RXPS_07_COLLECT_FRAME_KEY_2:
                    ws->frame_masking_nonce_04[1] = c;
                    if (c)
                        ws->all_zero_nonce = 0;
                    this->lws_rx_parse_state = LWS_RXPS_07_COLLECT_FRAME_KEY_3;
                    break;

                case LWS_RXPS_07_COLLECT_FRAME_KEY_3:
                    ws->frame_masking_nonce_04[2] = c;
                    if (c)
                        ws->all_zero_nonce = 0;
                    this->lws_rx_parse_state = LWS_RXPS_07_COLLECT_FRAME_KEY_4;
                    break;

                case LWS_RXPS_07_COLLECT_FRAME_KEY_4:
                    ws->frame_masking_nonce_04[3] = c;
                    if (c)
                        ws->all_zero_nonce = 0;
                    this->lws_rx_parse_state =
                        LWS_RXPS_PAYLOAD_UNTIL_LENGTH_EXHAUSTED;
                    ws->frame_mask_index = 0;
                    if (ws->rx_packet_length == 0)
                        return 0;
                    break;


                case LWS_RXPS_PAYLOAD_UNTIL_LENGTH_EXHAUSTED:
                    if (ws->all_zero_nonce)
                    {
                        this->rx_user_buffer_.append(1, c);
                    }
                    else
                    {
                        this->rx_user_buffer_.append(1, c ^ ws->frame_masking_nonce_04[(ws->frame_mask_index++) & 3]);
                    }
                    if (--ws->rx_packet_length == 0) {
                        this->lws_rx_parse_state = LWS_RXPS_NEW;
                        return 0;
                    }
            }
            return 0;

illegal_ctl_length:
            return -1;
        }

        bool Frame::Prepare4Parse()
        {
            this->recv_data_.clear();
            this->rx_user_buffer_.clear();
            this->pos_ = 0;
            this->lws_rx_parse_state = LWS_RXPS_NEW;
            memset(&this->ws_, 0, sizeof(this->ws_));
            return true;
        }

        int32_t Frame::AppendRecvData(const std::string& in)
        {
            this->recv_data_ += in;
            return 0;
        }

        int32_t Frame::ParseFrame(FrameInfo &fr, std::string& rest)
        {
            rest.clear();
            for(; this->pos_ < this->recv_data_.length(); this->pos_++)
            {
                int rc = this->libwebsocket_rx_sm(&this->ws_, this->recv_data_[this->pos_]);
                if (rc != 0)
                {
                    return -1;
                }
                if (rc == 0)
                {
                    if (this->lws_rx_parse_state == LWS_RXPS_NEW)
                    {
                        assert(this->ws_.rx_packet_length == 0);
                        if (this->ws_.opcode == LWS_WS_OPCODE_07__BINARY_FRAME)
                        {
                            fr.proto = LWS_WRITE_BINARY;
                        }
                        else if (this->ws_.opcode == LWS_WS_OPCODE_07__TEXT_FRAME)
                        {
                            fr.proto = LWS_WRITE_TEXT;
                        }
                        else if (this->ws_.opcode == LWS_WS_OPCODE_07__CONTINUATION)
                        {
                            fr.proto = LWS_WRITE_CONTINUATION;
                        }
                        else if (this->ws_.opcode == LWS_WS_OPCODE_07__CLOSE)
                        {
                            fr.proto = LWS_WRITE_CLOSE;
                        }
                        else
                        {
                            assert(0);
                        }
                        fr.reserved = this->ws_.rsv;
                        fr.fin= this->ws_.fin;
                        fr.payload = this->rx_user_buffer_;
                        rest = this->recv_data_.substr(this->pos_ + 1);
                        return 0;
                    }
                    else if (this->ws_.rx_packet_length == 0 and this->lws_rx_parse_state == LWS_RXPS_PAYLOAD_UNTIL_LENGTH_EXHAUSTED)
                    {
                        if (this->ws_.opcode == LWS_WS_OPCODE_07__CLOSE)
                        {
                            fr.proto = LWS_WRITE_CLOSE;
                        }
                        else if (this->ws_.opcode == LWS_WS_OPCODE_07__PING)
                        {
                            fr.proto = LWS_WRITE_PING;
                        }
                        else if (this->ws_.opcode == LWS_WS_OPCODE_07__PONG)
                        {
                            fr.proto = LWS_WRITE_PONG;
                        }
                        else
                        {
                            assert(0);
                        }
                        fr.payload = "";
                        fr.reserved = 0;
                        fr.fin = 1;
                        rest = this->recv_data_.substr(this->pos_ + 1);
                        return 0;
                    }
                }
            }
            return 1;
        }
    }
}

//#include <stdio.h>
//int main(int argc, char** argv)
//{
//    //tmp = argv[1];
//    //std::string in(argv[2]);
//
//    //std::string out = comm::websocket::Frame::MakeFrame(in,
//    //        comm::websocket::Frame::LWS_WRITE_TEXT,
//    //        true, true, comm::websocket::Frame::LWS_CLOSE_STATUS_NO_STATUS);
//    char s1[] = {'\xc1','\x88','\x7b','\x79','\x42','\x47','\x31','\x32','\x01','\xc7','\x3f','\x2d','\x42','\x47'};
//    int l1 = 14;
//    char s2[] = {'\xc1','\x93','\xbc','\x93','\x60','\x36','\xf6','\xd8','\x29','\x1d','\xf2','\xb8','\x66','\x27','\x95','\x56','\x70','\x20','\x28','\x9c','\x76','\x31','\xee','\x93','\x60'};
//    int l2 = 25;
//
//    comm::websocket::Frame f;
//    f.Prepare4Parse();
//    f.AppendRecvData(std::string(s2, l2) + std::string(s1, l1));
//    std::string rest;
//    comm::websocket::Frame::FrameInfo fi;
//    int rc = f.ParseFrame(fi, rest);
//    if (rc == 0)
//    {
//        comm::websocket::DeflateFrame df(true);
//        std::string out = df.Inflate(fi.payload);
//        printf("%s\n", out.c_str());
//    }
//    f.Prepare4Parse();
//    f.AppendRecvData(rest);
//    rc = f.ParseFrame(fi, rest);
//    if (rc == 0)
//    {
//        comm::websocket::DeflateFrame df(true);
//        std::string out = df.Inflate(fi.payload);
//        printf("%s\n", out.c_str());
//    }
//    return 0;
//}
