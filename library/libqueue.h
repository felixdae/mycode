#ifndef LIBQUEUE_H
#define LIBQUEUE_H 
#include "beanstalk-wrapper.hpp"

namespace comm
{
    namespace library
    {
        class QueueClient
        {
            public:
                QueueClient(const std::string& ip, uint16_t port, const std::string& queue_name);
                virtual ~QueueClient();
                int64_t EnQueue(const std::string& msg, uint32_t priority, uint32_t delay, uint32_t ttr);
                int64_t Front(uint32_t timeout, std::string& msg);
                int32_t Delete(int64_t msg_id);
                int32_t Release(int64_t msg_id, uint32_t priority, uint32_t delay);

                std::string GetErrorMsg() const
                {
                    return error_msg_;
                }
            private:
                bool CheckConnection();
                bool Init();
                Beanstalk::Client client_;
                std::string ip_;
                uint16_t port_;
                std::string queue_name_;
                std::string error_msg_;
                //Beanstalk::Job job_;
        };
    }
}
#endif /* LIBQUEUE_H */
