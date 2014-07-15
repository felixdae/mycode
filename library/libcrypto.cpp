#include <cstring>
#include <cstdio>
#include <cassert>
#include <arpa/inet.h>

#include <memory>

#include <openssl/md5.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>
#include <mcrypt.h>
#include "libcrypto.h"
#include "crc32.h"

namespace comm
{
    namespace library
    {
//        std::string Crypto::GetMode(Crypto::Mode mode)
//        {
//            switch (mode)
//            {
//                case Crypto::CBC:return "cbc";
//                case Crypto::CFB:return "cfb";
//                case Crypto::CTR:return "ctr";
//                case Crypto::ECB:return "ecb";,
//                case Crypto::NCFB:return "ncfb";
//                case Crypto::NOFB:return "nofb";
//                case Crypto::OFB:return "ofb";
//                default:assert(0);
//            }
//        }

        uint32_t Crypto::Crc32(const std::string& input)
        {
            uint32_t crc = 0 ^ 0xffffffff;
            uint8_t *p = (uint8_t *)input.c_str();
            int n = input.length();
            while (n--)
            {
                uint8_t ch = *(p++);
                crc = (crc >> 8) ^ crc32tab[(crc ^ (ch)) & 0xff];
            }
            return crc ^ 0xffffffff;
        }

        std::string Crypto::Md5(const std::string& input)
        {
            const uint8_t *p_in = (uint8_t *)(input.c_str());
            uint8_t tmp[80];
            MD5(p_in, input.length(), tmp);
            std::string output((char*)tmp, 16);
            return output;
        }

        std::string Crypto::HexDigest(const std::string& input)
        {
            std::string output;
            char hex[16] = {'0', '1', '2', '3', '4', '5', '6', '7',
                '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
            char c_tmp[10];
            uint32_t i = 0;
            for (; i < input.length(); i++)
            {
                c_tmp[0] = hex[(input[i]>>4)&0x0f];
                c_tmp[1] = hex[input[i]&0x0f];
                output.append(c_tmp, 2);
            }
            return output;
        }

        //        std::string Crypto::B64encodeUnit(const std::string& input)
        //        {
        //            assert(input.length() > 0);
        //            assert(input.length() <= 3);
        //            char b64_table[64] = {
        //                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        //                'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        //                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/',
        //            };
        //            std::string hold;
        //            uint8_t remain = (input[0] >> 2)&0x3f;
        //            if (input.length() >= 1)
        //            {
        //                hold.append(1, b64_table[remain]);
        //            //printf("%d, ", remain);
        //                remain = (input[0] << 4)&0x30;
        //            }
        //            if (input.length() == 1)
        //            {
        //                hold.append(1, b64_table[remain]);
        //                hold += "==";
        //                return hold;
        //            }
        //            if (input.length() >= 2)
        //            {
        //                remain ^= ((input[1] >> 4)&0x0f);
        //            //printf("%d, ", remain);
        //                hold.append(1, b64_table[remain]);
        //                remain = (input[1] << 2)&0x3c;
        //            }
        //            if (input.length() == 2)
        //            {
        //                hold.append(1, b64_table[remain]);
        //                hold += "=";
        //                return hold;
        //            }
        //            remain ^= ((input[2] >> 6)&0x03);
        //            //printf("%d, ", remain);
        //            hold.append(1, b64_table[remain]);
        //            remain = (input[2]&0x3f);
        //            //printf("%d\n", remain);
        //            hold.append(1, b64_table[remain]);
        //            return hold;
        //        }

        std::string Crypto::B64encodeUnit(const std::string& input)
        {
            assert(input.length() > 0);
            assert(input.length() <= 3);
            char b64_table[64] = {
                'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/',
            };

            std::string hold;
            uint32_t container = 0;
            memcpy((char*)(&container) + 1, input.c_str(), input.length());
            container = ntohl(container);

            //printf("%d\n", container);
            hold.append(1, b64_table[(container>>18)&0x0000003f]);
            hold.append(1, b64_table[(container>>12)&0x0000003f]);
            hold.append(1, b64_table[(container>>6)&0x0000003f]);
            hold.append(1, b64_table[(container>>0)&0x0000003f]);
            //printf("%d, %d, %d, %d\n", (container>>18)&0x0000003f, (container>>12)&0x0000003f, (container>>6)&0x0000003f, (container>>0)&0x0000003f);
            if (input.length() == 1)
            {
                hold[2] = '=';
                hold[3] = '=';
            }
            if (input.length() == 2)
            {
                hold[3] = '=';
            }
            return hold;
        }

        std::string Crypto::B64decodeUnit(const std::string& input)
        {
            //printf("%s\n", input.c_str());
            assert(input.length() == 4);
            int8_t b64_reverse_table[128] = {
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
                -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
                52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1,  0, -1, -1,
                -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
                15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
                -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
                41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1,
            };
            uint32_t container = (b64_reverse_table[(uint8_t)input[0]]<<18) + (b64_reverse_table[(uint8_t)input[1]]<<12);
            //printf("%u,%u,%u,%u\n",
            //        b64_reverse_table[(uint8_t)input[0]],
            //        b64_reverse_table[(uint8_t)input[1]],
            //        b64_reverse_table[(uint8_t)input[2]],
            //        b64_reverse_table[(uint8_t)input[3]]);
            int pn = 1;
            if (input[input.length() - 2] != '=')
            {
                pn++;
                container += (b64_reverse_table[(uint8_t)input[2]]<<6);
                if(input[input.length() - 1] != '=') 
                {
                    pn++;
                    container += (b64_reverse_table[(uint8_t)input[3]]<<0);
                }
            }
            //printf("%u\n", container);
            container = htonl(container);
            return std::string((char*)&container + 1, pn);
        }

        std::string Crypto::B64encode(const std::string& input)
        {
            uint32_t start = 0;
            std::string output;
            while (start < input.length())
            {
                if (start + 3 <= input.length())
                {
                    output += Crypto::B64encodeUnit(input.substr(start, 3));
                }
                else
                {
                    output += Crypto::B64encodeUnit(input.substr(start, 3));
                }
                start += 3;
            }
            //printf("%s\n%s\n", hold.c_str(), hold2.c_str());
            //memcpy(output, hold.c_str(), hold.length());
            //length = hold.length();
            return output;
        }

        std::string Crypto::B64decode(const std::string& input)
        {
            assert(input.length()%4 == 0);
            uint32_t start = 0;
            std::string output;
            while (start < input.length())
            {
                output += Crypto::B64decodeUnit(input.substr(start, 4));
                start += 4;
            }
            //memcpy(output, hold.c_str(), hold.length());
            //printf("%s, %s\n", input.c_str(), output);
            //length = hold.length();
            return output;
        }

        int32_t Crypto::AESEncrypt(const std::string& input, const std::string& key, std::string& output)
        {
            MCRYPT st_crypt = mcrypt_module_open((char*)"rijndael-128", NULL, (char*)"ncfb", NULL);
            if (MCRYPT_FAILED == st_crypt)
                return -1;
            //std::string hex_hash, hash;
            //Crypto::Md5(key, hash);
            std::string hex_hash = Crypto::HexDigest(Crypto::Md5(key));
            
            std::shared_ptr<char> iv(new char[mcrypt_enc_get_iv_size(st_crypt)]);
            memcpy(iv.get(), hex_hash.c_str(), mcrypt_enc_get_iv_size(st_crypt));

            if (mcrypt_generic_init(st_crypt, (void*)hex_hash.c_str(), hex_hash.length(), iv.get()) < 0)
            {
                mcrypt_module_close(st_crypt);
                return -1;
            }

            std::shared_ptr<char> buffer(new char[input.length() + 256]);
            memcpy(buffer.get(), input.c_str(), input.length());
            if (mcrypt_generic(st_crypt, buffer.get(), input.length()) != 0)
            {
                mcrypt_generic_deinit(st_crypt);
                mcrypt_module_close(st_crypt);
                return -1;
            }
            output.assign(buffer.get(), input.length());
            
            mcrypt_generic_deinit(st_crypt);
            mcrypt_module_close(st_crypt);
            return 0;
        }

        int32_t Crypto::AESDecrypt(const std::string& input, const std::string& key, std::string& output)
        {
            MCRYPT st_crypt = mcrypt_module_open((char*)"rijndael-128", NULL, (char*)"ncfb", NULL);
            if (MCRYPT_FAILED == st_crypt)
                return -1;
            //std::string hex_hash, hash;
            //Crypto::Md5(key, hash);
            std::string hex_hash = Crypto::HexDigest(Crypto::Md5(key));
            
            std::shared_ptr<char> iv(new char[mcrypt_enc_get_iv_size(st_crypt)]);
            memcpy(iv.get(), hex_hash.c_str(), mcrypt_enc_get_iv_size(st_crypt));

            if (mcrypt_generic_init(st_crypt, (void*)hex_hash.c_str(), hex_hash.length(), iv.get()) < 0)
            {
                mcrypt_module_close(st_crypt);
                return -1;
            }

            std::shared_ptr<char> buffer(new char[input.length() + 256]);
            memcpy(buffer.get(), input.c_str(), input.length());
            if (mdecrypt_generic(st_crypt, buffer.get(), input.length()) != 0)
            {
                mcrypt_generic_deinit(st_crypt);
                mcrypt_module_close(st_crypt);
                return -1;
            }
            output.assign(buffer.get(), input.length());
            
            mcrypt_generic_deinit(st_crypt);
            mcrypt_module_close(st_crypt);
            return 0;
        }

//        std::string Crypto::RsaPublicKeyEncode(const std::string& input, const std::string& pub_key, std::string& error_msg)
//        {
//            RSA *rsa= NULL;
//            BIO *keybio = BIO_new_mem_buf((void*)(pub_key.c_str()), pub_key.length());
//            if (keybio == NULL)
//            {
//                error_msg = "BIO_new_mem_buf error";
//                return "";
//            }
//            rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa, NULL, NULL);
//            if (rsa == NULL)
//            {
//                BIO_free(keybio);
//                error_msg = "PEM_read_bio_RSA_PUBKEY error";
//                return "";
//            }
//
//            std::string res;
//            uint32_t p = 0;
//            while (p < input.length())
//            {
//                int32_t in_len = p + (RSA_size(rsa) - 11) < input.length()? (RSA_size(rsa) - 11) : input.length() - p;
//                std::shared_ptr<uint8_t> out(new uint8_t[RSA_size(rsa)]);
//                int32_t out_len = RSA_public_encrypt(in_len, (const uint8_t*)(input.c_str() + p), out.get(), rsa, RSA_PKCS1_PADDING);
//                if (out_len == -1)
//                {
//                    BIO_free(keybio);
//                    RSA_free(rsa);
//                    error_msg = "RSA_public_encrypt error";
//                    return "";
//                }
//                p += in_len;
//                res += std::string((char*)out.get(), out_len);
//            }
//            BIO_free(keybio);
//            RSA_free(rsa);
//            return res;
//        }
//
//        std::string Crypto::RsaPrivateKeyDecode(const std::string& input, const std::string& pri_key, std::string& error_msg)
//        {
//            RSA *rsa= NULL;
//            BIO *keybio = BIO_new_mem_buf(pub_key.c_str(), pub_key.length());
//            if (keybio == NULL)
//            {
//                error_msg = "BIO_new_mem_buf error";
//                return "";
//            }
//            rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa, NULL, NULL);
//            if (rsa == NULL)
//            {
//                BIO_free(keybio);
//                error_msg = "PEM_read_bio_RSAPrivateKey error";
//                return "";
//            }
//
//            std::string res;
//            uint32_t p = 0;
//            while (p < input.length())
//            {
//                int32_t in_len = p + (RSA_size(rsa) - 11) < input.length()? (RSA_size(rsa) - 11) : input.length() - p;
//                std::shared_ptr<uint8_t> out(new uint8_t[RSA_size(rsa)]);
//                int32_t out_len = RSA_private_decrypt(in_len, (const uint8_t*)(input.c_str() + p), out.get(), rsa, RSA_PKCS1_PADDING);
//                if (out_len == -1)
//                {
//                    BIO_free(keybio);
//                    RSA_free(rsa);
//                    error_msg = "RSA_private_decrypt error";
//                    return "";
//                }
//                p += in_len;
//                res += std::string((char*)out.get(), out_len);
//            }
//            BIO_free(keybio);
//            RSA_free(rsa);
//            return res;
//        }
    }
}

