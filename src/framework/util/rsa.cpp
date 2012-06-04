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
#include <gmp.h>

void RSA::encrypt(char *msg, int size, const char* key)
{
    assert(size <= 128);

    mpz_t plain, c;
    mpz_init2(plain, 1024);
    mpz_init2(c, 1024);

    mpz_t e;
    mpz_init(e);
    mpz_set_ui(e,65537);

    mpz_t mod;
    mpz_init2(mod, 1024);
    mpz_set_str(mod, key, 10);

    mpz_import(plain, size, 1, 1, 0, 0, msg);
    mpz_powm(c, plain, e, mod);

    size_t count = (mpz_sizeinbase(c, 2) + 7)/8;
    memset(msg, 0, size - count);
    mpz_export(&msg[size - count], NULL, 1, 1, 0, 0, c);

    mpz_clear(c);
    mpz_clear(plain);
    mpz_clear(e);
    mpz_clear(mod);
}

void RSA::decrypt(char *msg, int size, const char *p, const char *q, const char *d)
{
    assert(size <= 128);

    mpz_t mp, mq, md, u, dp, dq, mod, c, v1, v2, u2, tmp;
    mpz_init2(mp, 1024);
    mpz_init2(mq, 1024);
    mpz_init2(md, 1024);
    mpz_init2(u, 1024);
    mpz_init2(dp, 1024);
    mpz_init2(dq, 1024);
    mpz_init2(mod, 1024);
    mpz_init2(c, 1024);
    mpz_init2(v1, 1024);
    mpz_init2(v2, 1024);
    mpz_init2(u2, 1024);
    mpz_init2(tmp, 1024);

    mpz_set_str(mp, p, 10);
    mpz_set_str(mq, q, 10);
    mpz_set_str(md, d, 10);

    mpz_t pm1,qm1;
    mpz_init2(pm1, 520);
    mpz_init2(qm1, 520);

    mpz_sub_ui(pm1, mp, 1);
    mpz_sub_ui(qm1, mq, 1);
    mpz_invert(u, mp, mq);
    mpz_mod(dp, md, pm1);
    mpz_mod(dq, md, qm1);

    mpz_mul(mod, mp, mq);

    mpz_import(c, size, 1, 1, 0, 0, msg);

    mpz_mod(tmp, c, mp);
    mpz_powm(v1, tmp, dp, mp);
    mpz_mod(tmp, c, mq);
    mpz_powm(v2, tmp, dq, mq);
    mpz_sub(u2, v2, v1);
    mpz_mul(tmp, u2, u);
    mpz_mod(u2, tmp, mq);
    if(mpz_cmp_si(u2, 0) < 0) {
        mpz_add(tmp, u2, mq);
        mpz_set(u2, tmp);
    }
    mpz_mul(tmp, u2, mp);
    mpz_set_ui(c, 0);
    mpz_add(c, v1, tmp);

    size_t count = (mpz_sizeinbase(c, 2) + 7)/8;
    memset(msg, 0, size - count);
    mpz_export(&msg[size - count], NULL, 1, 1, 0, 0, c);

    mpz_clear(c);
    mpz_clear(v1);
    mpz_clear(v2);
    mpz_clear(u2);
    mpz_clear(tmp);
    mpz_clear(pm1);
    mpz_clear(qm1);
    mpz_clear(mp);
    mpz_clear(mq);
    mpz_clear(md);
    mpz_clear(u);
    mpz_clear(dp);
    mpz_clear(dq);
    mpz_clear(mod);
}
