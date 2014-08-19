#include <algorithm>
#include "libstringhelper.h"

namespace comm
{
    namespace library
    {
        std::string StringHelper::HexDump(uint8_t *src, uint32_t len)
        {
            std::string res;
            char table[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};
            for (uint32_t i = 0; i < len; ++i)
            {
                uint8_t c = *(src + i);
                res.append(1, table[c>>4]);
                res.append(1, table[c&0x0f]);
            }
            return res;
        }

        std::string StringHelper::Upper(const std::string& src)
        {
            std::string res;
            for (auto c : src)
            {
                if (c >= 'a' and c <= 'z')
                {
                    res.append(1, c + ('A' - 'a'));
                }
                else
                {
                    res.append(1, c);
                }
            }
            return res;
        }

        std::string StringHelper::Lower(const std::string& src)
        {
            std::string res(src);
            //for (auto c : src)
            //{
            //    if (c >= 'A' and c <= 'Z')
            //    {
            //        res.append(1, c + ('a' - 'A'));
            //    }
            //    else
            //    {
            //        res.append(1, c);
            //    }
            //}
            //auto f = [](char c){return (c >= 'A' and c <= 'Z')? c + 'a' - 'A': c;};
            //auto f = [](char c){printf("%c, %d\n", c, __LINE__);return 'x';};
            //printf("%c%c\n", f('a'), f('b'));
            std::transform(res.begin(), res.end(), res.begin(), [](char c){return (c >= 'A' and c <= 'Z')? c + 'a' - 'A': c;});
            return res;
        }


        std::string StringHelper::Remove(const std::string& src, char target)
        {
            std::string res(src);
            res.erase(std::remove(res.begin(), res.end(), target), res.end());
            return res;
        }
        
        std::string StringHelper::Replace(const std::string& src, char from, char to)
        {
            std::string res(src);
            std::replace(res.begin(), res.end(), from, to);
            return res;
        }

        std::string StringHelper::LeftTrim(const std::string& src)
        {
            std::string blank = " \t\n\r\x0B" + std::string(1, '\0');
            auto p = src.find_first_not_of(blank);
            return p == std::string::npos? "" : src.substr(p);
        }

        std::string StringHelper::RightTrim(const std::string& src)
        {
            std::string rv(src.rbegin(), src.rend());
            std::string trv(StringHelper::LeftTrim(rv));
            return std::string(trv.rbegin(), trv.rend());
        }
                
        std::string StringHelper::Trim(const std::string& src)
        {
            return StringHelper::LeftTrim(StringHelper::RightTrim(src));
        }

        std::vector<std::string> StringHelper::Split(const std::string& src, char deli)
        {
            std::vector<std::string> res;
            size_t p = 0, q;
            while (p < src.length())
            {
                if ((q = src.substr(p).find(deli)) != std::string::npos)
                {
                    res.push_back(src.substr(p, q));
                    p += q + 1;
                }
                else
                {
                    res.push_back(src.substr(p));
                    p = src.length();
                }
            }
            if (*src.rbegin() == deli)
            {
                res.push_back("");
            }
            return res;
        }
        
        std::vector<std::string> StringHelper::SplitM(const std::string& src, char deli)
        {
            std::vector<std::string> res;
            std::string tmp(src);

            tmp.erase(std::unique(tmp.begin(), tmp.end(), [&deli](char  x, char y){return (x == deli and y == deli);}), tmp.end());

            return StringHelper::Split(tmp, deli);
        }

        std::string StringHelper::Join(const std::vector<std::string>& strs, char deli)
        {
            if (strs.empty())
                return "";
            if (strs.size() == 1)
            {
                return *strs.begin();
            }
            std::string res = *strs.begin();
            for_each(strs.begin() + 1, strs.end(), [deli,&res](const std::string t){res += std::string(1, deli) + t;});
            return res;
        }
    }
}
