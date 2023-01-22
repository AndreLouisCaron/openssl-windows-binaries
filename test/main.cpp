// -*- coding: utf-8 -*-

#include <openssl/ssl.h>
#include <iostream>

int main(int argc, char * argv[])
{
    std::cout
        << OpenSSL_version(OPENSSL_VERSION)
        << std::endl;
    return EXIT_SUCCESS;
}
