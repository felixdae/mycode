#include "gtest/gtest.h"
#include "libredisclient.h"

TEST(Redis, List)
{
    comm::library::RedisClient rc("10.0.1.27", 6379, 1);
    std::string out;
    ASSERT_EQ(rc.Push(comm::library::RedisClient::RedisConstant::kFromLeft, "testredis", "yyyuuuuu"), 0);
    ASSERT_EQ(rc.Pop(comm::library::RedisClient::RedisConstant::kFromRight, "testredis", out), 0);
    EXPECT_EQ(out, "yyyuuuuu");
    ASSERT_EQ(rc.Push(comm::library::RedisClient::RedisConstant::kFromLeft, "testredis", "12345"), 0);
    ASSERT_EQ(rc.Pop(comm::library::RedisClient::RedisConstant::kFromRight, "testredis", out), 0);
    EXPECT_EQ(out, "12345");
    ASSERT_EQ(rc.Pop(comm::library::RedisClient::RedisConstant::kFromRight, "testredis", out), 1);
}

//TEST(Redis, LPUSH)
//{
//    comm::library::RedisClient rc("10.0.1.27", 6379, 1);
//    ASSERT_EQ(rc.Push(comm::library::RedisClient::RedisConstant::kFromLeft, "testredis", "yyyuuuuu"), 0);
//}
