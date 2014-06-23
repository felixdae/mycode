#include <openssl/bn.h>
#include <stdint.h>
#include <memory>

//gcc -g -Wall -I /usr/include/ test_bignum.cpp -std=c++11 -lcrypto -lstdc++ -o testbn
int main(int argc, char** argv)
{
    BIGNUM *a = BN_new();
    BN_dec2bn(&a, argv[1]);
    
    uint32_t bns = BN_num_bytes(a);

    std::shared_ptr<uint8_t> bin(new uint8_t[bns]);
    BN_bn2bin(a, bin.get());
    char *hex = BN_bn2hex(a);
    char *dec = BN_bn2dec(a);
    OPENSSL_free(dec);
    OPENSSL_free(hex);
    BN_clear_free(a);
    return 0;
}

