#ifndef DEFLATE_FRAME_H
#define DEFLATE_FRAME_H 
#include <zlib.h>
#include <string>

namespace comm
{
    namespace websocket
    {

        class DeflateFrame
        {
            public:
                explicit DeflateFrame(bool for_server);
                virtual ~DeflateFrame();
                std::string Deflate(const std::string& in);
                std::string Inflate(const std::string& in);

                std::string GetErrorMsg() const
                {
                    return this->error_msg_;
                }
            private:
                bool Init();
                const int LWS_ZLIB_WINDOW_BITS = 15;
                const int LWS_ZLIB_MEMLEVEL = 8;

                std::string error_msg_;
                bool for_server_;
                z_stream zs_in;
                z_stream zs_out;
        };
    }
}
#endif /* DEFLATE-FRAME_H */
