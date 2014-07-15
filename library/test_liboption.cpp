#include <string>
#include <map>

#include "gtest/gtest.h"

#include "liboption.h"

TEST(Option, Normal)
{
    char argv_[][50] = 
    {
        "app",
        "-a",
        "xyz",
        "-b",
        "-c",
        "123"
    };
    int argc_ = sizeof(argv_) / sizeof(argv_[0]);
    char* pargv_[argc_];
    for (int i = 0; i < argc_; ++i)
    {
        pargv_[i] = argv_[i];
    }

    const char *acc = "a:bc:def";

    comm::library::CommandLineOption clo(argc_, pargv_, acc);
    EXPECT_TRUE(clo.HasOption('a'));
    EXPECT_TRUE(clo.HasOption('b'));
    EXPECT_TRUE(clo.HasOption('c'));
    EXPECT_FALSE(clo.HasOption('d'));
    EXPECT_FALSE(clo.HasOption('e'));

    EXPECT_EQ(clo.GetValue('a', ""), "xyz");
    EXPECT_EQ(clo.GetValue('c', -1), 123);
    EXPECT_EQ(clo.GetValue('b', -2), -2);
    EXPECT_EQ(clo.GetValue('d', "-2"), "-2");
    EXPECT_EQ(clo.GetValue('e', -4), -4);
}

//can not call getopt twice
//TEST(Option, Exception)
//{
//    char argv_[][50] = 
//    {
//        "app",
//        "-a",
//        "xyz",
//        "-b",
//        "-c",
//        //"123",
//        //"-g"
//    };
//    int argc_ = sizeof(argv_) / sizeof(argv_[0]);
//    char* pargv_[argc_];
//    for (int i = 0; i < argc_; ++i)
//    {
//        pargv_[i] = argv_[i];
//    }
//
//    const char *acc = ":a:bc:def";
//    //const char *acc = ":a:bc:def";//first : will suppress error
//
//    comm::library::CommandLineOption clo(argc_, pargv_, acc, false);
//    printf("%s\n", clo.GetErrorMsg().c_str());
////    EXPECT_TRUE(clo.HasOption('a'));
////    EXPECT_TRUE(clo.HasOption('b'));
////    EXPECT_TRUE(clo.HasOption('c'));
////    EXPECT_FALSE(clo.HasOption('d'));
////    EXPECT_FALSE(clo.HasOption('e'));
////
////    EXPECT_EQ(clo.GetValue('a', ""), "xyz");
////    EXPECT_EQ(clo.GetValue('c', -1), 123);
////    EXPECT_EQ(clo.GetValue('b', -2), -2);
////    EXPECT_EQ(clo.GetValue('d', "-2"), "-2");
////    EXPECT_EQ(clo.GetValue('e', -4), -4);
//}
