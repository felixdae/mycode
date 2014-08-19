#ifndef LIBLOG_H
#define LIBLOG_H 
#include <cstdio>

#include <string>
#include <memory>

namespace comm
{
    namespace library
    {
        class Logger
        {
            public:
                Logger();
                virtual ~Logger();
                bool Init(const std::string& file_name, uint32_t max_file_num, uint32_t max_file_size, bool write_to_console, bool debug);
                //int32_t LogDebug(const char* format, ...);
                //int32_t LogNotice(const char* format, ...);

                int32_t Log(bool log_debug, const char* format, ...);
                //int32_t SetLogDebug();
                int32_t ResetRotateDuty();
                static bool FileExist(const std::string& file_name);
            private:
                int32_t Rotate();
                FILE* fp_;

                std::string file_name_;
                uint32_t max_file_num_;
                uint32_t max_file_size_;
                bool log_debug_;
                bool rotate_duty_;
                bool write_to_console_;
                bool sync_;
        };

        class FunctionGuard
        {
            public:
                FunctionGuard(const std::string& func_name, Logger* logger);
                ~FunctionGuard();
            private:
                Logger* logger_;
                std::string func_name_;
                uint64_t enter_time_;
        };
    }
}

#endif /* LIBLOG_H */
