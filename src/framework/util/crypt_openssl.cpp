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

#include "crypt_openssl.h"
#include <framework/stdext/math.h>
#include <framework/core/logger.h>
#include <framework/core/resourcemanager.h>
#include <framework/platform/platform.h>
#include <framework/core/application.h>

#include <boost/functional/hash.hpp>

#include <openssl/rsa.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <openssl/bn.h>
#include <openssl/err.h>

#if OPENSSL_VERSION_NUMBER < 0x10100000L
#include <string.h>
#include <openssl/engine.h>

int RSA_set0_key(RSA *r, BIGNUM *n, BIGNUM *e, BIGNUM *d)
{
    /* If the fields n and e in r are NULL, the corresponding input
     * parameters MUST be non-NULL for n and e.  d may be
     * left NULL (in case only the public key is used).
     */
    if ((r->n == NULL && n == NULL)
        || (r->e == NULL && e == NULL))
        return 0;

    if (n != NULL) {
        BN_free(r->n);
        r->n = n;
    }
    if (e != NULL) {
        BN_free(r->e);
        r->e = e;
    }
    if (d != NULL) {
        BN_free(r->d);
        r->d = d;
    }

    return 1;
}

int RSA_set0_factors(RSA *r, BIGNUM *p, BIGNUM *q)
{
    /* If the fields p and q in r are NULL, the corresponding input
     * parameters MUST be non-NULL.
     */
    if ((r->p == NULL && p == NULL)
        || (r->q == NULL && q == NULL))
        return 0;

    if (p != NULL) {
        BN_free(r->p);
        r->p = p;
    }
    if (q != NULL) {
        BN_free(r->q);
        r->q = q;
    }

    return 1;
}

void RSA_get0_key(const RSA *r, const BIGNUM **n, const BIGNUM **e, const BIGNUM **d)
{
    if (n != NULL)
        *n = r->n;
    if (e != NULL)
        *e = r->e;
    if (d != NULL)
        *d = r->d;
}

void RSA_get0_factors(const RSA *r, const BIGNUM **p, const BIGNUM **q)
{
    if (p != NULL)
        *p = r->p;
    if (q != NULL)
        *q = r->q;
}

#endif

CryptOpenSSL g_crypt_openssl;

CryptOpenSSL::CryptOpenSSL()
{
    m_rsa = RSA_new();
}

CryptOpenSSL::~CryptOpenSSL()
{
    RSA_free(m_rsa);
}

std::string CryptOpenSSL::md5Encode(const std::string& decoded_string, bool upperCase)
{
    MD5_CTX c;
    MD5_Init(&c);
    MD5_Update(&c, decoded_string.c_str(), decoded_string.length());

    uint8_t md[MD5_DIGEST_LENGTH];
    MD5_Final(md, &c);

    char output[(MD5_DIGEST_LENGTH << 1) + 1];
    for(int32_t i = 0; i < (int32_t)sizeof(md); ++i)
        sprintf(output + (i << 1), "%.2X", md[i]);

    std::string result = output;
    if(upperCase)
        return result;

    std::transform(result.begin(), result.end(), result.begin(), tolower);
    return result;
}

std::string CryptOpenSSL::sha1Encode(const std::string& decoded_string, bool upperCase)
{
    SHA_CTX c;
    SHA1_Init(&c);
    SHA1_Update(&c, decoded_string.c_str(), decoded_string.length());

    uint8_t md[SHA_DIGEST_LENGTH];
    SHA1_Final(md, &c);

    char output[(SHA_DIGEST_LENGTH << 1) + 1];
    for(int32_t i = 0; i < (int32_t)sizeof(md); ++i)
        sprintf(output + (i << 1), "%.2X", md[i]);

    std::string result = output;
    if(upperCase)
        return result;

    std::transform(result.begin(), result.end(), result.begin(), tolower);
    return result;
}

std::string CryptOpenSSL::sha256Encode(const std::string& decoded_string, bool upperCase)
{
    SHA256_CTX c;
    SHA256_Init(&c);
    SHA256_Update(&c, decoded_string.c_str(), decoded_string.length());

    uint8_t md[SHA256_DIGEST_LENGTH];
    SHA256_Final(md, &c);

    char output[(SHA256_DIGEST_LENGTH << 1) + 1];
    for(int32_t i = 0; i < (int32_t)sizeof(md); ++i)
        sprintf(output + (i << 1), "%.2X", md[i]);

    std::string result = output;
    if(upperCase)
        return result;

    std::transform(result.begin(), result.end(), result.begin(), tolower);
    return result;
}

std::string CryptOpenSSL::sha512Encode(const std::string& decoded_string, bool upperCase)
{
    SHA512_CTX c;
    SHA512_Init(&c);
    SHA512_Update(&c, decoded_string.c_str(), decoded_string.length());

    uint8_t md[SHA512_DIGEST_LENGTH];
    SHA512_Final(md, &c);

    char output[(SHA512_DIGEST_LENGTH << 1) + 1];
    for(int32_t i = 0; i < (int32_t)sizeof(md); ++i)
        sprintf(output + (i << 1), "%.2X", md[i]);

    std::string result = output;
    if(upperCase)
        return result;

    std::transform(result.begin(), result.end(), result.begin(), tolower);
    return result;
}

void CryptOpenSSL::rsaGenerateKey(int bits, int e)
{
    BN_CTX *ctx = BN_CTX_new();
    BN_CTX_start(ctx);

    const BIGNUM *bn_n = BN_CTX_get(ctx), *bn_e = BN_CTX_get(ctx),
            *bn_d = BN_CTX_get(ctx), *bn_p = BN_CTX_get(ctx), *bn_q = BN_CTX_get(ctx);
    BIGNUM *bn_e_new = BN_CTX_get(ctx);

    RSA *rsa = RSA_new();
    BN_set_word(bn_e_new, e);
    RSA_generate_key_ex(rsa, bits, bn_e_new, nullptr);
    g_logger.info(stdext::format("%d bits (%d bytes) RSA key generated", bits, bits / 8));

    RSA_get0_key(rsa, &bn_n, &bn_e, &bn_d);
    RSA_get0_factors(rsa, &bn_p, &bn_q);
    g_logger.info(std::string("p = ") + BN_bn2dec(bn_p));
    g_logger.info(std::string("q = ") + BN_bn2dec(bn_q));
    g_logger.info(std::string("d = ") + BN_bn2dec(bn_d));
    g_logger.info(std::string("n = ") + BN_bn2dec(bn_n));
    g_logger.info(std::string("e = ") + BN_bn2dec(bn_e));

    RSA_free(rsa);

    BN_CTX_end(ctx);
}

void CryptOpenSSL::rsaSetPublicKey(const std::string& n, const std::string& e)
{
    BIGNUM *bn_n = nullptr, *bn_e = nullptr;

    BN_dec2bn(&bn_n, n.c_str());
    BN_dec2bn(&bn_e, e.c_str());

    RSA_set0_key(m_rsa, bn_n, bn_e, nullptr);
}

void CryptOpenSSL::rsaSetPrivateKey(const std::string& p, const std::string& q, const std::string& d)
{
    BIGNUM *bn_p = nullptr, *bn_q = nullptr, *bn_d = nullptr;

    BN_dec2bn(&bn_p, p.c_str());
    BN_dec2bn(&bn_q, q.c_str());
    BN_dec2bn(&bn_d, d.c_str());
    RSA_set0_factors(m_rsa, bn_p, bn_q);

    // calculate D from P and Q
    if (d.empty()) {
        const BIGNUM *bn_e_const;
        RSA_get0_key(m_rsa, nullptr, &bn_e_const, nullptr);

        if (bn_e_const != nullptr) {
            bn_d = BN_new();
            auto *bn_e = const_cast<BIGNUM *>(bn_e_const);

            BN_CTX *ctx = BN_CTX_new();
            BN_CTX_start(ctx);

            BIGNUM *bn_p_1 = BN_CTX_get(ctx), *bn_q_1 = BN_CTX_get(ctx), *bn_1 = BN_CTX_get(ctx), *bn_phi = BN_CTX_get(ctx);

            const std::string one = "1";
            BN_dec2bn(&bn_1, one.c_str());

            BN_sub(bn_p_1, bn_p, bn_1);
            BN_sub(bn_q_1, bn_q, bn_1);
            BN_mul(bn_phi, bn_p_1, bn_q_1, ctx);
            BN_mod_inverse(bn_d, bn_e, bn_phi, ctx);

            BN_CTX_end(ctx);
        } else {
            stdext::throw_exception("Cannot calculate RSA D without E set before by g_crypt.rsaSetPublicKey(n, e)");
            return;
        }
    }

    RSA_set0_key(m_rsa, nullptr, nullptr, bn_d);
}

bool CryptOpenSSL::rsaCheckKey()
{
    if(RSA_check_key(m_rsa))
        return true;
    else {
        ERR_load_crypto_strings();
        g_logger.error(stdext::format("RSA check failed - %s", ERR_error_string(ERR_get_error(), nullptr)));
        return false;
    }
}

bool CryptOpenSSL::rsaEncrypt(unsigned char *msg, int size)
{
    if(size != RSA_size(m_rsa))
        return false;

    return RSA_public_encrypt(size, msg, msg, m_rsa, RSA_NO_PADDING) != -1;
}

bool CryptOpenSSL::rsaDecrypt(unsigned char *msg, int size)
{
    if(size != RSA_size(m_rsa))
        return false;

    return RSA_private_decrypt(size, msg, msg, m_rsa, RSA_NO_PADDING) != -1;
}

int CryptOpenSSL::rsaGetSize()
{
    return RSA_size(m_rsa);
}

