/*
 * Copyright (c) 2010-2017 OTClient <https://github.com/edubart/otclient>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef CRYPT_GMP_H
#define CRYPT_GMP_H

#include "../stdext/types.h"
#include <string>

#include <boost/uuid/uuid.hpp>
#include <gmp.h>

class CryptGMP
{
public:
    CryptGMP();
    ~CryptGMP();

    void rsaGenerateKey(int bits, int e);
    void rsaSetPublicKey(const std::string& n, const std::string& e);
    void rsaSetPrivateKey(const std::string &p, const std::string &q, const std::string &d);
    bool rsaCheckKey();
    bool rsaEncrypt(unsigned char *msg, int size);
    bool rsaDecrypt(unsigned char *msg, int size);
    int rsaGetSize();

    static constexpr auto MODULUS_SIZE = 1024;
    static constexpr auto BLOCK_SIZE = MODULUS_SIZE / 8;

private:
    mpz_t rsaN, rsaE, rsaD;
};

extern CryptGMP g_crypt_gmp;

#endif
