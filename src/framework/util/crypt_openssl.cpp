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

static const std::string base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static inline bool is_base64(unsigned char c) { return (isalnum(c) || (c == '+') || (c == '/')); }

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
    // disabled because new OpenSSL changes broke
    /*
    RSA *rsa = RSA_new();
    BIGNUM *ebn = BN_new();
    BN_set_word(ebn, e);
    RSA_generate_key_ex(rsa, bits, ebn, nullptr);
    g_logger.info(stdext::format("%d bits (%d bytes) RSA key generated", bits, bits / 8));
    g_logger.info(std::string("p = ") + BN_bn2dec(m_rsa->p));
    g_logger.info(std::string("q = ") + BN_bn2dec(m_rsa->q));
    g_logger.info(std::string("d = ") + BN_bn2dec(m_rsa->d));
    g_logger.info(std::string("n = ") + BN_bn2dec(m_rsa->n));
    g_logger.info(std::string("e = ") + BN_bn2dec(m_rsa->e));
    BN_clear_free(ebn);
    RSA_free(rsa);
    */
}

void CryptOpenSSL::rsaSetPublicKey(const std::string& n, const std::string& e)
{
    BN_dec2bn(&m_rsa->n, n.c_str());
    BN_dec2bn(&m_rsa->e, e.c_str());

    // clear rsa cache
    if(m_rsa->_method_mod_n) { BN_MONT_CTX_free(m_rsa->_method_mod_n); m_rsa->_method_mod_n = NULL; }
}

void CryptOpenSSL::rsaSetPrivateKey(const std::string& p, const std::string& q, const std::string& d)
{
    BN_dec2bn(&m_rsa->p, p.c_str());
    BN_dec2bn(&m_rsa->q, q.c_str());
    BN_dec2bn(&m_rsa->d, d.c_str());

    // clear rsa cache
    if(m_rsa->_method_mod_p) { BN_MONT_CTX_free(m_rsa->_method_mod_p); m_rsa->_method_mod_p = NULL; }
    if(m_rsa->_method_mod_q) { BN_MONT_CTX_free(m_rsa->_method_mod_q); m_rsa->_method_mod_q = NULL; }
}

bool CryptOpenSSL::rsaCheckKey()
{
    // only used by server, that sets both public and private
    if(RSA_check_key(m_rsa)) {
        BN_CTX *ctx = BN_CTX_new();
        BN_CTX_start(ctx);

        BIGNUM *r1 = BN_CTX_get(ctx), *r2 = BN_CTX_get(ctx);
        BN_mod(m_rsa->dmp1, m_rsa->d, r1, ctx);
        BN_mod(m_rsa->dmq1, m_rsa->d, r2, ctx);

        BN_mod_inverse(m_rsa->iqmp, m_rsa->q, m_rsa->p, ctx);
        return true;
    }
    else {
        ERR_load_crypto_strings();
        g_logger.error(stdext::format("RSA check failed - %s", ERR_error_string(ERR_get_error(), NULL)));
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

