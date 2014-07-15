#ifndef LIBHTTPCLIENT_H
#define LIBHTTPCLIENT_H 

#include <string>
#include <map>

#include <curl/curl.h>

namespace comm
{
    namespace library
    {
        class HttpClient
        {
            public:
                HttpClient(uint32_t timeout);
                virtual ~HttpClient();
                int32_t Get(const std::string& url, uint32_t &http_code);
                int32_t Post(const std::string& url, const std::string& data, uint32_t &http_code);
                std::string GetResponse() const 
                {
                    return this->response_;
                }
                std::string GetErrorMsg() const
                {
                    return this->error_msg_;
                }
            public:
                static int32_t UrlEncode(const std::map<std::string, std::string>& data, std::string& url_encoded_data);
            private:
                CURL *h;

                uint32_t timeout_;
                std::string response_;
                std::string error_msg_;
                bool init_ok_;
                bool Init();
                static size_t DataReceivedCallback(char *ptr, size_t size, size_t nmemb, void  *userdata);
        };
    }
}

#endif /* LIBHTTPCLIENT_H */
