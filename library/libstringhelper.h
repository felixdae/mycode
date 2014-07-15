#ifndef LIBSTRINGHELPER_H
#define LIBSTRINGHELPER_H 
#include <string>
#include <vector>

namespace comm
{
    namespace library
    {
        class StringHelper
        {
            public:
                static std::string HexDump(uint8_t *src, uint32_t len);
                
                static std::string Upper(const std::string& src);
                static std::string Lower(const std::string& src);

                static std::string LeftTrim(const std::string& src);
                static std::string RightTrim(const std::string& src);
                static std::string Trim(const std::string& src);

                static std::string Remove(const std::string& src, char target);
                static std::string Replace(const std::string& src, char from, char to);

                static std::vector<std::string> Split(const std::string& src, char deli);
                static std::vector<std::string> SplitM(const std::string& src, char deli);
        };
    }
}
#endif /* LIBSTRINGHELPER_H */
