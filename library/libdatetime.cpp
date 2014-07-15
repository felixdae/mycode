#include <sys/time.h>

#include "libdatetime.h"

namespace comm
{
    namespace library
    {
        uint64_t DateTime::GetUnixTimestamp(bool micro)
        {
            const uint64_t micro_per_second = 1000000;
            struct timeval tv;

            gettimeofday(&tv, NULL);

            if (micro)
            {
                return tv.tv_sec * micro_per_second + tv.tv_usec;
            }
            return tv.tv_sec;
        }

        int32_t DateTime::FormatFromTimestamp(int32_t timestamp, std::string& formatted)
        {
            const char *iso8601 = "%F %T";//"%Y-%m-%d %H:%M:%S";
            struct tm dt;
//            size_t strftime(char *s, size_t max, const char *format,
//                                           const struct tm *tm);
            time_t ts = timestamp;
            localtime_r(&ts, &dt);

            char out[128];
            int32_t rc = strftime(out, sizeof(out), iso8601, &dt);
            if (rc <= 0)
            {
                return -1;
            }
            formatted.assign(out, rc);
            return 0;
        }

        int32_t DateTime::GetTimestampFromDateTime(const std::string& date_time, int32_t &timestamp)
        {
            const char *iso8601 = "%F %T";//"%Y-%m-%d %H:%M:%S";
            //char *strptime(const char *s, const char *format, struct tm *tm);

            struct tm dt;
            char *p = strptime(date_time.c_str(), iso8601, &dt);
            if (*p != '\0')
            {
                return -1;
            }

            timestamp = mktime(&dt);
            return 0;
        }
    }
}

