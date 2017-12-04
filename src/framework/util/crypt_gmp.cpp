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

#include "crypt_gmp.h"
#include <framework/core/application.h>
CryptGMP g_crypt_gmp;

auto notImplementedMessage = "Not implemented in GMP. Compile OTClient with OpenSSL (-DUSE_GMP=0).";

CryptGMP::CryptGMP()
{
    mpz_init(rsaN);
    mpz_init2(rsaD, MODULUS_SIZE);
    mpz_init2(rsaE, MODULUS_SIZE);
}

CryptGMP::~CryptGMP()
{
    mpz_clear(rsaN);
    mpz_clear(rsaD);
    mpz_clear(rsaE);
}
std::string CryptGMP::md5Encode(const std::string& decoded_string, bool upperCase)
{
    stdext::throw_exception(notImplementedMessage);
    return notImplementedMessage;
}

std::string CryptGMP::sha1Encode(const std::string& decoded_string, bool upperCase)
{
    stdext::throw_exception(notImplementedMessage);
    return notImplementedMessage;
}

std::string CryptGMP::sha256Encode(const std::string& decoded_string, bool upperCase)
{
    stdext::throw_exception(notImplementedMessage);
    return notImplementedMessage;
}

std::string CryptGMP::sha512Encode(const std::string& decoded_string, bool upperCase)
{
    stdext::throw_exception(notImplementedMessage);
    return notImplementedMessage;
}

void CryptGMP::rsaGenerateKey(int bits, int e)
{
    stdext::throw_exception(notImplementedMessage);
}

void CryptGMP::rsaSetPublicKey(const std::string& n, const std::string& e)
{
    mpz_set_str(rsaN, n.c_str(), 10);
    mpz_set_str(rsaE, e.c_str(), 10);
}

void CryptGMP::rsaSetPrivateKey(const std::string& p, const std::string& q, const std::string& d)
{
    mpz_t rsaP, rsaQ;
    mpz_init2(rsaP, MODULUS_SIZE);
    mpz_init2(rsaQ, MODULUS_SIZE);

    mpz_set_str(rsaP, p.c_str(), 10);
    mpz_set_str(rsaQ, q.c_str(), 10);
    mpz_set_str(rsaD, d.c_str(), 10);

    // n = p * q
    mpz_mul(rsaN, rsaP, rsaQ);

    // calculate D from P and Q
    if (d.empty()) {
        // WARNING! rsaE can be not set - can't check it with GMP!
        mpz_t p_1, q_1, phi;
        mpz_init2(p_1, MODULUS_SIZE);
        mpz_init2(q_1, MODULUS_SIZE);
        mpz_init2(phi, MODULUS_SIZE);

        mpz_sub_ui(p_1, rsaP, 1);
        mpz_sub_ui(q_1, rsaQ, 1);

        // phi = (p -1)(q - 1)
        mpz_mul(phi, p_1, q_1);

        // d = e^-1 mod (p - 1)(q - 1)
        mpz_invert(rsaD, rsaE, phi);

        mpz_clear(p_1);
        mpz_clear(q_1);
        mpz_clear(phi);
    }

    mpz_clear(rsaP);
    mpz_clear(rsaQ);
}

bool CryptGMP::rsaEncrypt(unsigned char *msg, int size)
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

bool CryptGMP::rsaDecrypt(unsigned char *msg, int size)
{
    mpz_t c, m;
    mpz_init2(c, MODULUS_SIZE);
    mpz_init2(m, MODULUS_SIZE);

    mpz_import(c, BLOCK_SIZE, 1, 1, 0, 0, msg);

    // m = c^d mod n
    mpz_powm(m, c, rsaD, rsaN);

    size_t count = (mpz_sizeinbase(m, 2) + 7) / 8;
    memset(msg, 0, BLOCK_SIZE - count);
    mpz_export(msg + (BLOCK_SIZE - count), nullptr, 1, 1, 0, 0, m);

    mpz_clear(c);
    mpz_clear(m);

    return true;
}

bool CryptGMP::rsaCheckKey()
{
    stdext::throw_exception(notImplementedMessage);
    return false;
}

int CryptGMP::rsaGetSize()
{
    return BLOCK_SIZE;
}

