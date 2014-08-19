#include <unistd.h>
#include <stdarg.h>

#include "liblog.h"
#include "libdatetime.h"

namespace comm
{
    namespace library
    {
        FunctionGuard::FunctionGuard(const std::string& func_name, Logger* logger)
            :logger_(logger), func_name_(func_name), 
            enter_time_(comm::library::DateTime::GetUnixTimestamp(true))
        {
            this->logger_->Log(false, "enter function: %s", func_name.c_str());
        }

        FunctionGuard::~FunctionGuard()
        {
            this->logger_->Log(false, "leave function: %s, use time: %lld", this->func_name_.c_str(), comm::library::DateTime::GetUnixTimestamp(true) - this->enter_time_);
        }

        Logger::Logger()
            :log_debug_(false),
            rotate_duty_(true),
            sync_(true)
        {
        }

        Logger::~Logger()
        {
            if (this->fp_ != NULL)
            {
                fclose(this->fp_);
            }
        }

        bool Logger::Init(const std::string& file_name, uint32_t max_file_num, uint32_t max_file_size, bool write_to_console, bool debug)
        {
            this->file_name_ = file_name;
            this->max_file_num_ = max_file_num;
            this->max_file_size_ = max_file_size;
            this->write_to_console_ = write_to_console;
            this->log_debug_ = debug;
            this->fp_ = fopen(file_name.c_str(), "a+");
            if (this->fp_ == NULL)
                return false;
            return true;
        }

//        int32_t Logger::SetLogDebug()
//        {
//            this->log_debug_ = true;
//            return 0;
//        }

        int32_t Logger::ResetRotateDuty()
        {
            this->rotate_duty_ = false;
            return 0;
        }

        int32_t Logger::Log(bool log_debug, const char* format, ...)
        {
            if (log_debug == true and this->log_debug_ == false)
            {
                return 0;
            }

            const uint32_t buf_len = 1024;
            char buf[buf_len];

            va_list ap;
            va_start(ap, format);
            vsnprintf(buf, sizeof(buf), format, ap);
            buf[buf_len - 1] = '\0';
            va_end(ap);

            std::string formatted;
            comm::library::DateTime::FormatFromTimestamp(comm::library::DateTime::GetUnixTimestamp(), formatted);
            
            if (this->write_to_console_)
            {
                fprintf(stdout, "[%s]%s\n", formatted.c_str(), buf);
                if (this->sync_)
                {
                    fflush(stdout);
                }
            }
            fprintf(this->fp_, "[%s]%s\n", formatted.c_str(), buf);
            if (this->sync_)
            {
                fflush(this->fp_);
            }

            if (static_cast<uint32_t>(ftell(this->fp_)) > this->max_file_size_)
            {
                this->Rotate();
            }

            return 0;
        }

        int32_t Logger::Rotate()
        {
            fclose(this->fp_);
            if (this->rotate_duty_)
            {
                std::string from_file_name;
                std::string to_file_name;
                for (uint32_t i = this->max_file_num_ - 1; i != 0; --i)
                {
                    from_file_name = this->file_name_ + "." + std::to_string(i);
                    to_file_name = this->file_name_ + "." + std::to_string(i + 1);
                    if (Logger::FileExist(from_file_name))
                    {
                        rename(from_file_name.c_str(), to_file_name.c_str());
                    }

                }
                from_file_name = this->file_name_;
                to_file_name = this->file_name_ + ".1";
                if (Logger::FileExist(from_file_name))
                {
                    rename(from_file_name.c_str(), to_file_name.c_str());
                }
            }
            this->fp_ = fopen(this->file_name_.c_str(), "a+");
            return 0;
        }

        bool Logger::FileExist(const std::string& file_name)
        {
            if (access(file_name.c_str(), F_OK) != -1)
                return true;
            return false;
        }

        //        int32_t Logger::LogDebug(const char* format, ...)
        //        {
        //            this->Log(
        //        }
        //
        //        int32_t Logger::LogNotice(const char* format, ...)
        //        {
        //            va_list ap;
        //            va_start(ap, format);
        //            this->Log(ap);
        //            va_end(ap);
        //        }
    }
}

