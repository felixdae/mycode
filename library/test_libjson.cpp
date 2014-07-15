//#include <cstdio>
//#include "libjson.h"
//
//int main(int argc, char** argv)
//{
//    //const std::string jd("{\"a\":\"123\",\"b\":\"678\",\"90\":[\"jj\",\"kk\",\"ll\",\"mm\"],\"m78\":{\"xx\":1,\"yy\":1,\"zz\":1,\"uu\":[1,2,3,4,\"bool\"]}}");
//    const std::string jd(argv[1]);
//
//    comm::library::JsonPath jp;
//    jp.Append("web-app").Append("servlet").Append(1).Append("init-param").Append("mailHostOverride");
//
//    comm::library::JsonParser jx(jd);
//    std::string tt;
//    jx.GetValueByPath(jp,tt);
//
//    printf("%s\n", tt.c_str());
//    
//    return 0;
//}

#include <string>
#include <map>

#include "gtest/gtest.h"

#include "libjson.h"

TEST(Json, Parser)
{
    const std::string jd("{\"a\":\"123\",\"b\":678,\"90\":[\"jj\",\"kk\",\"ll\",\"mm\"],\"m78\":{\"xx\":\"用户名不能为空\",\"yy\":1,\"zz\":1,\"uu\":[1,2,3,4,\"bool\"]}}");
    comm::library::JsonPath jp;
    jp.Append("m78").Append("uu").Append(4);
    //printf("%s\n", jp.GetHumanPath().c_str());

    comm::library::JsonParser jx(jd);
    std::string tt;
    ASSERT_EQ(0, jx.GetValueByPath(jp, tt));
    EXPECT_EQ("bool", tt);

    jp.Retain(1);
    jp.Append("xx");
    //printf("%s\n", jp.GetHumanPath().c_str());
    ASSERT_EQ(0, jx.GetValueByPath(jp, tt));
    EXPECT_EQ("用户名不能为空", tt);
    
    jp.Retain(0);
    jp.Append("b");
    //printf("%s\n", jp.GetHumanPath().c_str());
    int64_t ut;
    ASSERT_EQ(0, jx.GetValueByPath(jp, ut));
    EXPECT_EQ(678, ut);
    //printf("%s\n", tt.c_str());
}

