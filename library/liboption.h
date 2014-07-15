#ifndef LIBOPTION_H
#define LIBOPTION_H 
#include <string>
#include <map>

#include <unistd.h>
namespace comm
{
    namespace library
    {
        class CommandLineOption
        {
            public:
                CommandLineOption(int argc, char **argv, const char *candidate, bool suppress_error = true);
                virtual ~CommandLineOption();
                bool HasOption(char opt);
                int32_t GetValue(char opt, int32_t default_value);
                std::string GetValue(char opt, const std::string& default_value);
                
                std::string GetErrorMsg() const
                {
                    return this->error_msg_;
                }
            private:
                bool Init(int argc, char **argv, const char *candidate, bool suppress_error);
                int initialized_;
                std::map<char, std::string> opt_map_;
                std::string error_msg_;
        };
    }
}
#endif /* LIBOPTION_H */
