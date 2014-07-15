#ifndef LIBCRYPTO_H
#define LIBCRYPTO_H 

#include <stdint.h>

#include <string>

//#include <mcrypty.h>
//#include <openssl.h>

namespace comm
{
    namespace library
    {
        class Crypto
        {
            public:
//                enum Mode
//                {
//                    CBC,
//                    CFB,
//                    CTR,
//                    ECB,
//                    NCFB,
//                    NOFB,
//                    OFB,
//                };
            public:
                //des
                //static int32_t DESEncrypt(const std::string& input, const std::string& key, Crypto::Mode mode, char* output, uint32_t &length);
                //static int32_t DESDecrypt(const std::string& input, const std::string& key, Crypto::Mode mode, char* output, uint32_t &length);
                ////3des
                //static int32_t 3DESEncrypt(const std::string& input, const std::string& key, Crypto::Mode mode, char* output, uint32_t &length);
                //static int32_t 3DESDecrypt(const std::string& input, const std::string& key, Crypto::Mode mode, char* output, uint32_t &length);
                ////aes
                static int32_t AESEncrypt(const std::string& input, const std::string& key, std::string& output);
                static int32_t AESDecrypt(const std::string& input, const std::string& key, std::string& output);
                //aes rijndael-128 (32): cbc cfb ctr ecb ncfb nofb ofb 
                //des (8):               cbc cfb ctr ecb ncfb nofb ofb 
                //tripledes (24):        cbc cfb ctr ecb ncfb nofb ofb 
                //static int32_t AESEncrypt(const std::string& input, const std::string& key, Crypto::Mode mode, char* output, uint32_t &length);
                //static int32_t AESDecrypt(const std::string& input, const std::string& key, Crypto::Mode mode, char* output, uint32_t &length);
                //rsa
                //bas64 encode/decode
                //md5
                static std::string B64encode(const std::string& input);
                static std::string B64decode(const std::string& input);
                static std::string Md5(const std::string& input);
                static std::string HexDigest(const std::string& input);
                //crc32
                static uint32_t Crc32(const std::string& input);

//                static std::string RsaPublicKeyEncode(const std::string& input, const std::string& pub_key, std::string& error_msg);
//                static std::string RsaPrivateKeyDecode(const std::string& input, const std::string& pri_key, std::string& error_msg);
//                
//                static std::string RsaPrivateKeyEncode(const std::string& input, const std::string& Pri_key, std::string& error_msg);
//                static std::string RsaPublicKeyDecode(const std::string& input, const std::string& pub_key, std::string& error_msg);
            private:
//                static std::string GetMode(Crypto::Mode mode);
                static std::string B64encodeUnit(const std::string& input);
                static std::string B64decodeUnit(const std::string& input);
        };
    }
}
#endif /* LIBCRYPTO_H */
