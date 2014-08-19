#ifndef LIBREDISCLIENT_H
#define LIBREDISCLIENT_H 

#include <string>
#include "hiredis/hiredis.h"
namespace comm
{
    namespace library
    {
        class RedisClient
        {
            public:
                enum class RedisConstant
                {
                    kFromLeft,
                    kFromRight,
                };
                RedisClient(const std::string& ip, uint16_t port, time_t timeout);
                ~RedisClient();
                int32_t Push(RedisConstant dir, const std::string& queue, const std::string& val);
                int32_t Pop(RedisConstant dir, const std::string& queue, std::string& ret);
                int32_t LPush(const std::string& queue, const std::string& val)
                {
                    return this->Push(RedisConstant::kFromLeft, queue, val);
                }
                int32_t RPop(const std::string& queue, std::string& ret)
                {
                    return this->Pop(RedisConstant::kFromRight, queue, ret);
                }

                std::string GetErrorMsg() const
                {
                    return this->error_msg_;
                }
            private:
                bool inited_;
                redisContext *c_;
                std::string error_msg_;
                //redisReply *reply_;
        };
    }
}
#endif /* LIBREDISCLIENT_H */
