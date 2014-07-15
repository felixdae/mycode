#ifndef LIBDATETIME_H
#define LIBDATETIME_H 
#include <string>

namespace comm
{
    namespace library
    {
        class DateTime
        {
            private:
                DateTime();
            public:
                virtual ~DateTime();

                static uint64_t GetUnixTimestamp(bool micro = false);
                static int32_t FormatFromTimestamp(int32_t timestamp, std::string& formatted);
                static int32_t GetTimestampFromDateTime(const std::string& date_time, int32_t &timestamp);
        };
    }
}
#endif /* LIBDATETIME_H */
