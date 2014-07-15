#include <stdio.h>
#include <time.h>
#include <math.h>
#include "gtest/gtest.h"

#include "libdatetime.h"

TEST(DateTime, GetUnixTimestamp)
{
    EXPECT_TRUE(abs(comm::library::DateTime::GetUnixTimestamp() - time(NULL)) < 1);
//    printf("%llu, %llu, %u", comm::library::DateTime::GetUnixTimestamp(true), comm::library::DateTime::GetUnixTimestamp(), time(NULL));
    EXPECT_TRUE(abs(comm::library::DateTime::GetUnixTimestamp(true) - 1000000 * (uint64_t)time(NULL)) < 1000000);
}

TEST(DateTime, FormatFromTimestamp)
{
    std::string formatted;
    ASSERT_EQ(0, comm::library::DateTime::FormatFromTimestamp(0, formatted));
    EXPECT_EQ("1970-01-01 08:00:00", formatted);
    ASSERT_EQ(0, comm::library::DateTime::FormatFromTimestamp(1400000000, formatted));
    EXPECT_EQ("2014-05-14 00:53:20", formatted);
}


TEST(DateTime, GetTimestampFromDateTime)
{
    int32_t ts;
    ASSERT_EQ(0, comm::library::DateTime::GetTimestampFromDateTime("1970-01-01 08:00:00", ts));
    EXPECT_EQ(0, ts);
    ASSERT_EQ(0, comm::library::DateTime::GetTimestampFromDateTime("1970-01-01 00:00:00", ts));
    EXPECT_EQ(-8*60*60, ts);
    ASSERT_EQ(0, comm::library::DateTime::GetTimestampFromDateTime("2014-05-14 00:53:20", ts));
    EXPECT_EQ(1400000000, ts);

    ts = time(NULL);
    int32_t tts;
    std::string formatted;
    ASSERT_EQ(0, comm::library::DateTime::FormatFromTimestamp(ts, formatted));
    ASSERT_EQ(0, comm::library::DateTime::GetTimestampFromDateTime(formatted, tts));
    EXPECT_EQ(tts, ts);
    
    formatted = "2012-05-24 20:50:20";
    std::string tf;
    ASSERT_EQ(0, comm::library::DateTime::GetTimestampFromDateTime(formatted, tts));
    ASSERT_EQ(0, comm::library::DateTime::FormatFromTimestamp(tts, tf));
    EXPECT_EQ(tf, formatted);
}
