#include <string>
#include <map>

#include "gtest/gtest.h"

#include "libstringhelper.h"

TEST(StringHelper, Split)
{
    std::vector<std::string> sv = comm::library::StringHelper::Split(":xx:", ':');
    EXPECT_EQ(3, sv.size());
    EXPECT_EQ(sv[0], "");
    EXPECT_EQ(sv[1], "xx");
    EXPECT_EQ(sv[2], "");

    sv = comm::library::StringHelper::Split("", ':');
    EXPECT_EQ(0, sv.size());
    
    sv = comm::library::StringHelper::Split("xxx:", ':');
    EXPECT_EQ(2, sv.size());
    EXPECT_EQ("xxx", sv[0]);
    EXPECT_EQ("", sv[1]);
    
    sv = comm::library::StringHelper::Split("xxx::yyy", ':');
    EXPECT_EQ(3, sv.size());
    EXPECT_EQ("xxx", sv[0]);
    EXPECT_EQ("", sv[1]);
    EXPECT_EQ("yyy", sv[2]);
}

TEST(StringHelper, Join)
{
    std::vector<std::string> vec = {"123","abc","$%^"};
    EXPECT_EQ(comm::library::StringHelper::Join(vec, ','), "123,abc,$%^");

    vec = {"dfg"};
    EXPECT_EQ(comm::library::StringHelper::Join(vec, ','), "dfg");
    
    vec = {};
    EXPECT_EQ(comm::library::StringHelper::Join(vec, ','), "");
}

TEST(StringHelper, SplitM)
{
    std::vector<std::string> sv = comm::library::StringHelper::SplitM(":xx:", ':');
    EXPECT_EQ(3, sv.size());
    EXPECT_EQ(sv[0], "");
    EXPECT_EQ(sv[1], "xx");
    EXPECT_EQ(sv[2], "");

    sv = comm::library::StringHelper::SplitM("", ':');
    EXPECT_EQ(0, sv.size());
    
    sv = comm::library::StringHelper::SplitM("xxx:", ':');
    EXPECT_EQ(2, sv.size());
    EXPECT_EQ("xxx", sv[0]);
    EXPECT_EQ("", sv[1]);
    
    sv = comm::library::StringHelper::SplitM("xxx::yyy", ':');
    EXPECT_EQ(2, sv.size());
    EXPECT_EQ("xxx", sv[0]);
    EXPECT_EQ("yyy", sv[1]);
    
    sv = comm::library::StringHelper::SplitM(":::yyy", ':');
    EXPECT_EQ(2, sv.size());
    EXPECT_EQ("", sv[0]);
    EXPECT_EQ("yyy", sv[1]);
}

TEST(StringHelper, HexDump)
{
    EXPECT_EQ("0a345be3", comm::library::StringHelper::HexDump((uint8_t*)"\x0a\x34\x5b\xe3", 4));
}

TEST(StringHelper, Upper)
{
    EXPECT_EQ("0A345BE3", comm::library::StringHelper::Upper("0a345be3"));
}

TEST(StringHelper, Lower)
{
    EXPECT_EQ("0a345be3", comm::library::StringHelper::Lower("0A345BE3"));
}

TEST(StringHelper, RightTrim)
{
    EXPECT_EQ(std::string(5, '\0') + "\r\t   " + "0a345be3", comm::library::StringHelper::RightTrim(std::string(5, '\0') + "\r\t   0a345be3\r\t\t\t\t\t   "));
}

TEST(StringHelper, LeftTrim)
{
    EXPECT_EQ("0a345be3\r\t\t\t\t\t   ", comm::library::StringHelper::LeftTrim(std::string(5, '\0') + "\r\t   0a345be3\r\t\t\t\t\t   "));
}

TEST(StringHelper, Trim)
{
    EXPECT_EQ("0a345be3", comm::library::StringHelper::Trim(std::string(5, '\0') + "\r\t   0a345be3\r\t\t\t\t\t   "));
}

TEST(StringHelper, Remove)
{
    EXPECT_EQ("statc std::strng Replace(const std::strng& src, char from, char to);", comm::library::StringHelper::Remove("static std::string Replace(const std::string& src, char from, char to);", 'i'));
}

TEST(StringHelper, Replace)
{
    EXPECT_EQ("statec std::streng Replace(const std::streng& src, char from, char to);", comm::library::StringHelper::Replace("static std::string Replace(const std::string& src, char from, char to);", 'i', 'e'));
}
