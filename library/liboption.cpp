#include <stdexcept>
#include "liboption.h"

namespace comm
{
    namespace library
    {
        CommandLineOption::CommandLineOption(int argc, char **argv, const char* candidate, bool suppress_error)
            :initialized_(false)
        {
            initialized_ = this->Init(argc, argv, candidate, suppress_error);
        }

        CommandLineOption::~CommandLineOption()
        {
        }

        bool CommandLineOption::Init(int argc, char **argv, const char *candidate, bool suppress_error)
        {
            if (suppress_error)
            {
                opterr = 0;
            }
            else
            {
                opterr = 1;
            }
            int oc;
            while ((oc = getopt(argc, argv, candidate)) != -1)
            {
                switch (oc)
                {
                    case '?':
                        if (candidate[0] == ':')
                        {
                            this->error_msg_ = "unknown option: " + std::string(1, optopt);
                        }
                        else
                        {
                            this->error_msg_ = "unknown option: " + std::string(1, optopt) + " or no argument supplied for option: " + std::string(1, optopt);
                        }
                        throw std::runtime_error(this->error_msg_);
                        return false;
                    case ':':
                        this->error_msg_ = "no argument supplied for option: " + std::string(1, optopt);
                        throw std::runtime_error(this->error_msg_);
                        return false;
                    default:
                        if (optarg != NULL)
                        {
                            this->opt_map_[oc] = optarg;
                        }
                        else
                        {
                            this->opt_map_[oc] = std::string(1, '\0');
                        }
                }
            }
            return true;
        }

        bool CommandLineOption::HasOption(char opt)
        {
            if (this->initialized_ == false)
                return false;
            if (this->opt_map_.count(opt) == 0)
                return false;
            return true;
        }

        int32_t CommandLineOption::GetValue(char opt, int32_t default_value)
        {
            if (this->HasOption(opt) and this->opt_map_[opt] != std::string(1, '\0'))
            {
                return std::strtol(this->opt_map_[opt].c_str(), NULL, 10);
            }
            return default_value;
        }

        std::string CommandLineOption::GetValue(char opt, const std::string& default_value)
        {
            if (this->HasOption(opt) and this->opt_map_[opt] != std::string(1, '\0'))
            {
                return this->opt_map_[opt];
            }
            return default_value;
        }
    }
}
