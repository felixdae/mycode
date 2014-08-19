#include <stdexcept>
#include "deflate_frame.h"

namespace comm
{
    namespace websocket
    {
        DeflateFrame::DeflateFrame(bool for_server)
            :for_server_(for_server)
        {
            this->Init();
        }

        bool DeflateFrame::Init()
        {
            this->zs_in.zalloc = this->zs_out.zalloc = Z_NULL;
            this->zs_in.zfree = this->zs_out.zfree = Z_NULL;
            this->zs_in.opaque = this->zs_out.opaque = Z_NULL;
            int n = inflateInit2(&this->zs_in, -LWS_ZLIB_WINDOW_BITS);
            if (n != Z_OK) {
                this->error_msg_ = "inflateInit2 returned " + std::to_string(n);
                return false;
            }
            n = deflateInit2(&this->zs_out,
                    (this->for_server_ ?
                     1:Z_DEFAULT_COMPRESSION),
                    Z_DEFLATED,
                    -LWS_ZLIB_WINDOW_BITS, LWS_ZLIB_MEMLEVEL,
                    Z_DEFAULT_STRATEGY);
            if (n != Z_OK) {
                this->error_msg_ = "deflateInit2 returned " + std::to_string(n);
                return false;
            }
            return true;
        }

        DeflateFrame::~DeflateFrame()
        {
            inflateEnd(&this->zs_in);
            deflateEnd(&this->zs_out);
        }


        std::string DeflateFrame::Inflate(const std::string& in)
        {
            /*
             * inflate the incoming payload
             */
            std::string tin(in);
            tin.append(2, '\0');
            tin.append(2, '\xff');
            //z_stream zs_in;
            zs_in.next_in = (uint8_t*)tin.c_str();
            zs_in.avail_in = tin.length();

            int32_t out_buf_len = 32;
            uint8_t* out_buf = (uint8_t*)malloc(out_buf_len);
            zs_in.avail_out = out_buf_len;
            zs_in.next_out = out_buf;

            while (1) 
            {
                int n = inflate(&zs_in, Z_SYNC_FLUSH);
                switch (n)
                {
                    case Z_NEED_DICT:
                    case Z_STREAM_ERROR:
                    case Z_DATA_ERROR:
                    case Z_MEM_ERROR:
                        /*
                         * screwed.. close the connection...
                         * we will get a destroy callback to take care
                         * of closing nicely
                         */
                        free(out_buf);
                        throw std::runtime_error("zlib error inflate " + std::to_string(n) + ", " + zs_in.msg);
                }

                if (zs_in.avail_out)
                    break;

                out_buf_len *= 2;
                out_buf = (unsigned char *)realloc(out_buf, out_buf_len);
                if (!out_buf)
                {
                    throw std::runtime_error("Out of memory");
                }
                zs_in.next_out = out_buf + (out_buf_len/2);
                zs_in.avail_out = out_buf_len/2;
            }

            /* rewrite the buffer pointers and length */
            std::string out_str((const char*)out_buf, out_buf_len - zs_in.avail_out);
            free(out_buf);
            return out_str;
        }

        std::string DeflateFrame::Deflate(const std::string& in)
        {
            /*
             * deflate the outgoing payload
             */
            //z_stream zs_out;
            zs_out.next_in = (uint8_t*)in.c_str();
            zs_out.avail_in = in.length();

            int32_t out_buf_len = 32;
            uint8_t* out_buf = (uint8_t*)malloc(out_buf_len);
            zs_out.avail_out = out_buf_len;
            zs_out.next_out = out_buf;

            while (1) 
            {
                int n = deflate(&zs_out, Z_SYNC_FLUSH);
                if (n == Z_STREAM_ERROR) 
                {
                    /*
                     * screwed.. close the connection... we will
                     * get a destroy callback to take care of
                     * closing nicely
                     */
                    free(out_buf);
                    throw std::runtime_error("zlib error deflate" + std::to_string(n) + ", " + zs_in.msg);
                }

                if (zs_out.avail_out)
                    break;

                out_buf_len *= 2;
                out_buf = (uint8_t *)realloc(out_buf, out_buf_len);
                if (!out_buf)
                {
                    throw std::runtime_error("Out of memory");
                }
                zs_out.next_out = out_buf + (out_buf_len/2);
                zs_out.avail_out = out_buf_len/2;
            }
            std::string out_str((const char*)out_buf, out_buf_len - zs_out.avail_out - 4);
            //std::string o1 = std::string((const char*)out_buf, out_buf_len - zs_out.avail_out);
            //printf("%d:%s\n%d:%s\n", out_str.length(), out_str.c_str(), o1.length(), o1.c_str());
            free(out_buf);
            return out_str;
        }
    }
}

//int main(int argc, char** argv)
//{
//    comm::websocket::DeflateFrame df(true);
//
//    uint8_t buf[] = {
//        0x2a, 0x2e, 0x86, 0x83, 0x44, 0x54, 0x00, 0x00,
//        0x2a, 0xc6, 0x29, 0x03, 0x00,
//        0xc2, 0x2d, 0x03, 0x00,
//    };
//    std::string in;
//    in = std::string((char*)buf, 8);
//    printf("%s\n", df.Inflate(in).c_str());
//    in = std::string((char*)(buf+8), 5);
//    printf("%s\n", df.Inflate(in).c_str());
//    in = std::string((char*)(buf+8+5), 4);
//    printf("%s\n", df.Inflate(in).c_str());
//    return 0;
//}
