//#include "libhttpclient.h"
//#include <string>
//#include <map>
//
//int main(int argc, char **argv)
//{
//    comm::library::HttpClient client(1);
//    int32_t rc = 0;
//    uint32_t http_code;
//    rc = client.Get("http://10.0.1.27/test.php1", http_code);
//    printf("%d, %u, %s\n", rc, http_code, client.GetResponse().c_str());
//    
//    std::map<std::string, std::string> ll;
//    ll["ab"] = "33-? $11";
//    ll["123"] = "jkj%%6./12";
//
//    std::string en;
//    comm::library::HttpClient::UrlEncode(ll, en);
//
//    rc = client.Post("http://10.0.1.27/test.php", en, http_code);
//    printf("%d, %u, %s\n", rc, http_code, client.GetResponse().c_str());
//
//    //printf("%s\n", en.c_str());
//    return 0;
//}
#include <string>
#include <map>

#include "gtest/gtest.h"

#include "libhttpclient.h"

TEST(HttpClient, UrlEncode)
{
    std::map<std::string, std::string> ll;

    ll["abc"] = "33-? $11";
    std::string en;
    comm::library::HttpClient::UrlEncode(ll, en);
    EXPECT_EQ("abc=33-%3F%20%2411", en);//"abc=33-%3F+%2411"

    ll["abc"] = "jkj%%6./12";
    comm::library::HttpClient::UrlEncode(ll, en);
    EXPECT_EQ("abc=jkj%25%256.%2F12", en);
}

TEST(HttpClient, Get)
{
    comm::library::HttpClient client(1);
    uint32_t http_code;
    int32_t rc = client.Get("http://www.baidu.com", http_code);
    EXPECT_EQ(rc, 0);
    EXPECT_EQ(http_code, 200);
    //printf("%s\n", client.GetResponse().c_str());
}

TEST(HttpClient, Post)
{
    //std::map<std::string, std::string> param;
    //param["wd"] = "america";
    //std::string en;
    //comm::library::HttpClient::UrlEncode(param, en);

    //comm::library::HttpClient client(55555);
    //uint32_t http_code;
    //int32_t rc = client.Post("http://acm.whu.edu.cn/blog/login.php?job=verify", en, http_code);
    //EXPECT_EQ(rc, 0);
    //EXPECT_EQ(http_code, 200);
    //printf("%s\n", client.GetResponse().c_str());
}

