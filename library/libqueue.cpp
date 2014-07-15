#include <cassert>
#include <stdexcept>
#include "libqueue.h"

namespace comm
{
    namespace library
    {
        QueueClient::QueueClient(const std::string& ip, uint16_t port, const std::string& queue_name)
            :client_(), ip_(ip), port_(port), queue_name_(queue_name)
        {
            this->Init();
        }

        QueueClient::~QueueClient()
        {
            //if (client_.is_connected())
            //{
            client_.disconnect();
            //}
        }

        bool QueueClient::Init()
        {
            try
            {
                client_.disconnect();
                client_.connect(this->ip_, this->port_, 1);
                assert(!this->queue_name_.empty());
                assert(client_.use(this->queue_name_));
                assert(client_.watch(this->queue_name_));
                assert(client_.ignore("default"));
            }
            catch (std::runtime_error &e)
            {
                this->error_msg_ = e.what();
            }
            return true;
        }

        bool QueueClient::CheckConnection()
        {
            if (!this->client_.is_connected())
            {
                //this->client_.reconnect();
                this->Init();
            }
            if (!this->client_.is_connected())
            {
                this->error_msg_ += " connection is broken";
                return false;
            }
            return true;
        }

        int64_t QueueClient::EnQueue(const std::string& msg, uint32_t priority, uint32_t delay, uint32_t ttr)
        {
            if (this->CheckConnection())
            {
                return this->client_.put((char*)(msg.c_str()), msg.length(), priority, delay, ttr);
            }
            return -1;
        }

        int64_t QueueClient::Front(uint32_t timeout, std::string& msg)
        {
            if (this->CheckConnection())
            {
                Beanstalk::Job job;
                if (this->client_.reserve(job, timeout))
                {
                    msg = job.body();
                    return job.id();
                }
                return 0;
            }
            return -1;
        }

        int32_t QueueClient::Delete(int64_t msg_id)
        {
            if (this->CheckConnection())
            {
                if (this->client_.del(msg_id))
                {
                    return 0;
                }
                return 1;
            }
            return -1;
        }
     
        int32_t QueueClient::Release(int64_t msg_id, uint32_t priority, uint32_t delay)
        {
            if (this->CheckConnection())
            {
                if (this->client_.release(msg_id, priority, delay))
                {
                    return 0;
                }
                return 1;
            }
            return -1;

        }

    }
}

