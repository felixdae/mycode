//#include <cstdio>
//#include <stdint.h>
//
//#include <string>
//
//#include "libcrypto.h"
//
//int main(int argc, char** argv)
//{
////    const std::string input(argv[1]);
//    //printf ("%u\n", comm::library::Crypto::Crc32(input));
//    std::string output;
//    //comm::library::Crypto::Md5(input, (uint8_t*)output);
//    //comm::library::Crypto::HexDigest(std::string(output, 16), output);
//    std::string in("Man is distinguished");//not only by his reason, but by this singular passion from other animals, which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable generation of knowledge, exceeds the short vehemence of any carnal pleasure.");
//    //comm::library::Crypto::B64encode(, output, len);
//    comm::library::Crypto::B64encode(in, output);
//    std::string out(output);
//    comm::library::Crypto::B64decode(out, output);
//    printf ("%s\n%s\n%s\n", in.c_str(),out.c_str(),output.c_str());
//    return 0;
//}

#include <string>
#include <map>

#include "gtest/gtest.h"

#include "libcrypto.h"

TEST(Crypto, Crc32)//equivalent to php crc32()
{
    EXPECT_EQ(comm::library::Crypto::Crc32("over time again, what a fuck life"), 1246448345);
}

TEST(Crypto, Md5)
{
    std::string input("Man is \"distinguished);//not only by\x7f\12\45 his reason, but \r\n" + std::string("\0", 1) + std::string("\x7b by this singular passion from other animals, which is a"));
    EXPECT_EQ(comm::library::Crypto::HexDigest(comm::library::Crypto::Md5(input)), "fffa87c52022b7e2492aee95b50563fe");
}

TEST(Crypto, Sha1)
{
    std::string input("Man is \"distinguishm other animals, which is a");
    EXPECT_EQ(comm::library::Crypto::HexDigest(comm::library::Crypto::Sha1(input)), "68a5f1fc7772596800acc04cb0e654246ce2e7ca");
}

TEST(Crypto, Base64)
{
    std::string input("Man is \"distinguished);//not only by\x7f\12\45 his reason, but \r\n" + std::string("\0", 1) + std::string("\x7b by this singular passion from other animals, which is a"));
//    std::string encode, decode;
    std::string expect("TWFuIGlzICJkaXN0aW5ndWlzaGVkKTsvL25vdCBvbmx5IGJ5fwolIGhpcyByZWFzb24sIGJ1dCANCgB7IGJ5IHRoaXMgc2luZ3VsYXIgcGFzc2lvbiBmcm9tIG90aGVyIGFuaW1hbHMsIHdoaWNoIGlzIGE=");

//    ASSERT_EQ(0, comm::library::Crypto::B64encode(input, encode));
    EXPECT_EQ(expect, comm::library::Crypto::B64encode(input));
    //ASSERT_EQ(0, comm::library::Crypto::B64decode(encode, decode));

    EXPECT_EQ(input, comm::library::Crypto::B64decode(expect));
}

TEST(Crypto, AES)
{
    std::string input("Man is \"distinguished);//not only by\x7f\12\45 his reason, but \r\n" + std::string("\0", 1) + std::string("\x7b by this singular passion from other animals, which is a"));
    std::string encode, decode;

    std::string key="hello, key88.";
    ASSERT_EQ(0, comm::library::Crypto::AESEncrypt(input, key, encode));
    EXPECT_NE(input, encode);
    ASSERT_EQ(0, comm::library::Crypto::AESDecrypt(encode, key, decode));
    EXPECT_EQ(decode, input);
}

