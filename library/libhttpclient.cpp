#include "libhttpclient.h"

namespace comm
{
    namespace library
    {
        HttpClient::HttpClient(uint32_t timeout)
            :timeout_(timeout), init_ok_(false)
        {
            this->init_ok_ = this->Init();
        }

        size_t HttpClient::DataReceivedCallback(char *ptr, size_t size, size_t nmemb, void  *userdata)
        {
            HttpClient *pt = (HttpClient*)userdata;
            pt->response_.append(ptr, size * nmemb);
            return size * nmemb;
        }

        bool HttpClient::Init()
        {
            CURLcode rc = curl_global_init(CURL_GLOBAL_ALL);
            if (rc != 0)
            {
                this->error_msg_ = std::string(__FILE__) + ":" + std::to_string(__LINE__) + "," + std::to_string(rc);
                return false;
            }

            this->h = curl_easy_init();
            if (this->h == NULL)
            {
                this->error_msg_ = std::string(__FILE__) + ":" + std::to_string(__LINE__);
                return false;
            }

            rc = curl_easy_setopt(this->h, CURLOPT_WRITEFUNCTION, HttpClient::DataReceivedCallback);
            if (rc != 0)
            {
                this->error_msg_ = std::string(__FILE__) + ":" + std::to_string(__LINE__) + "," + std::to_string(rc);
                return false;
            }

            rc = curl_easy_setopt(this->h, CURLOPT_WRITEDATA, this);
            if (rc != 0)
            {
                this->error_msg_ = std::string(__FILE__) + ":" + std::to_string(__LINE__) + "," + std::to_string(rc);
                return false;
            }

            rc = curl_easy_setopt(this->h, CURLOPT_TIMEOUT, this->timeout_);
            if (rc != 0)
            {
                this->error_msg_ = std::string(__FILE__) + ":" + std::to_string(__LINE__) + "," + std::to_string(rc);
                return false;
            }

            return true;
        }

        int32_t HttpClient::Get(const std::string& url, uint32_t &http_code)
        {
            if (!this->init_ok_)
            {
                return -1;
            }
            CURLcode rc = curl_easy_setopt(this->h, CURLOPT_URL, url.c_str());
            if (rc != 0)
            {
                this->error_msg_ = std::string(__FILE__) + ":" + std::to_string(__LINE__) + "," + std::to_string(rc);
                return -1;
            }

            rc = curl_easy_perform(this->h);
            if (rc != 0)
            {
                this->error_msg_ = std::string(__FILE__) + ":" + std::to_string(__LINE__) + "," + std::to_string(rc);
                return -1;
            }

            rc = curl_easy_getinfo(this->h, CURLINFO_RESPONSE_CODE, &http_code);
            if (rc != 0)
            {
                this->error_msg_ = std::string(__FILE__) + ":" + std::to_string(__LINE__) + "," + std::to_string(rc);
                return -1;
            }
            return 0;
        }

        int32_t HttpClient::Post(const std::string& url, const std::string& data, uint32_t &http_code)
        {
            if (!this->init_ok_)
            {
                return -1;
            }
            CURLcode rc = curl_easy_setopt(this->h, CURLOPT_URL, url.c_str());
            if (rc != 0)
            {
                this->error_msg_ = std::string(__FILE__) + ":" + std::to_string(__LINE__) + "," + std::to_string(rc);
                return -1;
            }

            rc = curl_easy_setopt(this->h, CURLOPT_POSTFIELDSIZE, data.length());
            if (rc != 0)
            {
                this->error_msg_ = std::string(__FILE__) + ":" + std::to_string(__LINE__) + "," + std::to_string(rc);
                return -1;
            }
            rc = curl_easy_setopt(this->h, CURLOPT_COPYPOSTFIELDS, data.c_str());
            if (rc != 0)
            {
                this->error_msg_ = std::string(__FILE__) + ":" + std::to_string(__LINE__) + "," + std::to_string(rc);
                return -1;
            }

            rc = curl_easy_perform(this->h);
            if (rc != 0)
            {
                this->error_msg_ = std::string(__FILE__) + ":" + std::to_string(__LINE__) + "," + std::to_string(rc);
                return -1;
            }

            rc = curl_easy_getinfo(this->h, CURLINFO_RESPONSE_CODE, &http_code);
            if (rc != 0)
            {
                this->error_msg_ = std::string(__FILE__) + ":" + std::to_string(__LINE__) + "," + std::to_string(rc);
                return -1;
            }
            return 0;
        }

        HttpClient::~HttpClient()
        {
            curl_easy_cleanup(this->h);
            curl_global_cleanup();
            this->init_ok_ = false;
        }

        int32_t HttpClient::UrlEncode(const std::map<std::string, std::string>& data, std::string& url_encoded_data)
        {
            url_encoded_data.clear();
            for(auto it = data.begin(); it != data.end(); ++it)
            {
                char *key = curl_easy_escape(NULL, it->first.c_str(), it->first.length());
                char *value = curl_easy_escape(NULL, it->second.c_str(), it->second.length());
                url_encoded_data += std::string(key) + "=" + std::string(value) + "&";
                curl_free(key);
                curl_free(value);
            }
            if (*(url_encoded_data.rbegin()) == '&')
            {
                url_encoded_data.erase(url_encoded_data.length() - 1);
            }
            return 0;
        }
    }
}
