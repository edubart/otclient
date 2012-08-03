/*
 * Copyright (c) 2010-2012 OTClient <https://github.com/edubart/otclient>
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

#include "rsa.h"
#include <openssl/bn.h>
#include <openssl/err.h>

Rsa g_rsa;

Rsa::Rsa()
{
    m_rsa = RSA_new();
}

Rsa::~Rsa()
{
    RSA_free(m_rsa);
}

void Rsa::setPublic(const char *n, const char *e)
{
    BN_dec2bn(&m_rsa->n, n);
    BN_dec2bn(&m_rsa->e, e);
}

void Rsa::setPrivate(const char *p, const char *q, const char *d)
{
    BN_dec2bn(&m_rsa->p, p);
    BN_dec2bn(&m_rsa->q, q);
    BN_dec2bn(&m_rsa->d, d);
}

bool Rsa::check() // only used by server, that sets both public and private
{
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

bool Rsa::encrypt(unsigned char *msg, int size)
{
    assert(size <= 128);
    return RSA_public_encrypt(size, msg, msg, m_rsa, RSA_NO_PADDING) != -1;
}

bool Rsa::decrypt(unsigned char *msg, int size)
{
    assert(size <= 128);
    return RSA_private_decrypt(size, msg, msg, m_rsa, RSA_NO_PADDING) != -1;
}
