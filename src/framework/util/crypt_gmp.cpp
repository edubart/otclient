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

#include "rsa_gmp.h"
#include <framework/stdext/math.h>
#include <framework/core/logger.h>
#include <framework/core/resourcemanager.h>
#include <framework/platform/platform.h>
#include <framework/core/application.h>

#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <boost/functional/hash.hpp>

#ifndef USE_GMP

#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/bn.h>
#include <openssl/err.h>

#endif

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static inline bool is_base64(unsigned char c) { return (isalnum(c) || (c == '+') || (c == '/')); }

RsaGMP g_crypt_gmp;

RsaGMP::RsaGMP()
{
    mpz_init(rsaN);
    mpz_init2(rsaD, MODULUS_SIZE);
    mpz_init2(rsaE, MODULUS_SIZE);
}

RsaGMP::~RsaGMP()
{
    mpz_clear(rsaN);
    mpz_clear(rsaD);
    mpz_clear(rsaE);
}

std::string RsaGMP::md5Encode(const std::string& decoded_string, bool upperCase)
{
}

std::string RsaGMP::sha1Encode(const std::string& decoded_string, bool upperCase)
{
}

std::string RsaGMP::sha256Encode(const std::string& decoded_string, bool upperCase)
{
}

std::string RsaGMP::sha512Encode(const std::string& decoded_string, bool upperCase)
{
}

void RsaGMP::rsaGenerateKey(int bits, int e)
{
}

void RsaGMP::rsaSetPublicKey(const std::string& n, const std::string& e)
{
    mpz_set_str(rsaN, n.c_str(), 10);
    mpz_set_str(rsaE, e.c_str(), 10);
}

void RsaGMP::rsaSetPrivateKey(const std::string& p, const std::string& q, const std::string& d)
{
    mpz_t rsaP, rsaQ;
    mpz_init2(rsaP, MODULUS_SIZE);
    mpz_init2(rsaQ, MODULUS_SIZE);

    mpz_set_str(rsaP, p.c_str(), 10);
    mpz_set_str(rsaQ, q.c_str(), 10);
    mpz_set_str(rsaD, d.c_str(), 10);

    // n = p * q
    mpz_mul(rsaN, rsaP, rsaQ);

    mpz_clear(rsaP);
    mpz_clear(rsaQ);
}

bool RsaGMP::rsaEncrypt(char *msg, int size)
{
    mpz_t c, m;
    mpz_init2(c, MODULUS_SIZE);
    mpz_init2(m, MODULUS_SIZE);

    mpz_import(m, BLOCK_SIZE, 1, 1, 0, 0, msg);

    // c = m^e mod n
    mpz_powm(c, m, rsaE, rsaN);

    size_t count = (mpz_sizeinbase(m, 2) + 7) / 8;
    memset(msg, 0, BLOCK_SIZE - count);
    mpz_export(msg + (BLOCK_SIZE - count - 1), nullptr, 1, 1, 0, 0, c);

    mpz_clear(c);
    mpz_clear(m);

    return true;
}

bool RsaGMP::rsaDecrypt(char *msg, int size)
{
    mpz_t c, m;
    mpz_init2(c, MODULUS_SIZE);
    mpz_init2(m, MODULUS_SIZE);

    mpz_import(c, BLOCK_SIZE, 1, 1, 0, 0, msg);

    // m = c^d mod n
    mpz_powm(m, c, rsaD, rsaN);

    size_t count = (mpz_sizeinbase(m, 2) + 7) / 8;
    memset(msg, 0, BLOCK_SIZE - count);
    mpz_export(msg + (BLOCK_SIZE - count - 1), nullptr, 1, 1, 0, 0, m);

    mpz_clear(c);
    mpz_clear(m);

    return true;
}

bool RsaGMP::rsaCheckKey()
{
}

bool RsaGMP::rsaEncrypt(unsigned char *msg, int size)
{
    if(size != RSA_size(m_rsa))
        return false;
    return RSA_public_encrypt(size, msg, msg, m_rsa, RSA_NO_PADDING) != -1;
}

bool RsaGMP::rsaDecrypt(unsigned char *msg, int size)
{
    if(size != RSA_size(m_rsa))
        return false;
    return RSA_private_decrypt(size, msg, msg, m_rsa, RSA_NO_PADDING) != -1;
}

int RsaGMP::rsaGetSize()
{
    return BLOCK_SIZE;
}

