#include <cassert>
#include "libredisclient.h"
#include "libscopeguard.h"

namespace comm
{
    namespace library
    {
        RedisClient::RedisClient(const std::string& ip, uint16_t port, time_t timeout)
            :inited_(false), c_(nullptr)
        {
            struct timeval tv = {timeout, 0};
            this->c_ = redisConnectWithTimeout(ip.c_str(), port, tv);
            if (this->c_->err)
            {
                this->error_msg_ = this->c_->errstr;
                return;
            }
            if (REDIS_OK != redisSetTimeout(this->c_, tv))
            {
                this->error_msg_ = this->c_->errstr;
                return;
            }
            this->inited_ = true;
        }

        RedisClient::~RedisClient()
        {
            if (this->c_)
            {
                redisFree(this->c_);
            }
        }
        
        int32_t RedisClient::Push(RedisConstant dir, const std::string& queue, const std::string& val)
        {
            if (!this->inited_)
                return -1;

            std::string command;
            if (dir == RedisConstant::kFromLeft)
            {
                command += "LPUSH";
            }
            else if (dir == RedisConstant::kFromRight)
            {
                command += "RPUSH";
            }
            else
            {
                assert(0);
            }
            //command += queue + " " + val;

            redisReply *reply = (redisReply*)redisCommand(this->c_, "%s %s %s", command.c_str(), queue.c_str(), val.c_str());
            ON_SCOPE_EXIT([&reply](){if (reply){freeReplyObject(reply);}});

            if (reply == NULL)
            {
                this->error_msg_ = std::string(this->c_->errstr);
                return -1;
            }
            if (reply->type != REDIS_REPLY_INTEGER)
            {
                if (reply->str)
                    this->error_msg_ = std::string(reply->str, reply->len);
                //freeReplyObject(reply);
                return -1;
            }
             //freeReplyObject(reply);
            return 0;
        }

        int32_t RedisClient::Pop(RedisConstant dir, const std::string& queue, std::string& ret)
        {
            if (!this->inited_)
                return -1;

            std::string command;
            if (dir == RedisConstant::kFromLeft)
            {
                command += "LPOP";
            }
            else if (dir == RedisConstant::kFromRight)
            {
                command += "RPOP";
            }
            else
            {
                assert(0);
            }
            //command += queue;

            redisReply *reply = (redisReply*)redisCommand(this->c_, "%s %s", command.c_str(), queue.c_str());
            ON_SCOPE_EXIT([&reply](){if (reply){freeReplyObject(reply);}});
            if (reply == NULL)
            {
                this->error_msg_ = std::string(this->c_->errstr);
                return -1;
            }
            if (reply->type == REDIS_REPLY_NIL)
            {
                //freeReplyObject(reply);
                return 1;
            }
            if (reply->type != REDIS_REPLY_STRING)
            {
                if (reply->str)
                    this->error_msg_ = std::string(reply->str, reply->len);
                //freeReplyObject(reply);
                return -1;
            }
            ret = std::string(reply->str, reply->len);
            //freeReplyObject(reply);
            return 0;
        }
    }
}
