//#include <unistd.h>
//#include "libqueue.h"
//
//int main()
//{
//
//    comm::library::QueueClient client("10.0.1.27", 11300, "lt");
//
//    int64_t id = client.EnQueue("ffffffffffffffkkkkkkkkkkkkk", 1234, 0, 120);
//    printf("%lld\n", id);
//
//    std::string msg;
//
//    while (1)
//    {
//        id = client.Front(msg, 10);
//        printf("%lld, %s\n", id, msg.c_str());
//        //printf("del result: %d\n", client.Delete(id));
//        sleep(100);
//        printf("rel result: %d\n", client.Release(id, 1234, 0));
//        sleep(100);
//        msg.clear();
//    }
////    id = client.Front(msg, 10);
////    printf("%lld\n, %s", id, msg.c_str());
//
//    return 0;
//
//}

#include <string>
#include <map>
#include <ctime>

#include "gtest/gtest.h"

#include "libqueue.h"

TEST(Queue, Beanstalk)
{
    std::string tube("lt_");
    tube += std::to_string(time(NULL));
    comm::library::QueueClient client("10.0.1.27", 11300, tube);

    ASSERT_GT(client.EnQueue("hello, world", 2048, 0, 60), 0);
    ASSERT_GT(client.EnQueue("hello, world, 11", 1024, 0, 60), 0);

    std::string msg;
    int64_t id1 = client.Front(0, msg);
    EXPECT_EQ(id1 > 0, true);
    EXPECT_EQ(msg, "hello, world, 11");
    
    int64_t id2 = client.Front(0, msg);
    EXPECT_GT(id2, 0);
    EXPECT_EQ(msg, "hello, world");

    EXPECT_EQ(0, client.Delete(id1));
    EXPECT_EQ(0, client.Release(id2, 2048, 0));

    id1 = client.Front(0, msg);
    EXPECT_EQ(id1, id2);
    EXPECT_EQ(msg, "hello, world");

    EXPECT_EQ(0, client.Delete(id1));
}
