/*
 * Copyright (c) 2010-2022 OTClient <https://github.com/edubart/otclient>
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

#pragma once

#include <string>

#include "uuid.h"

#ifdef USE_GMP
#include <gmp.h>
#else
using RSA = struct rsa_st;
#endif

class Crypt
{
public:
    Crypt();
    ~Crypt();

    std::string base64Encode(const std::string_view decoded_string);
    std::string base64Decode(const std::string_view encoded_string);
    std::string xorCrypt(const std::string_view buffer, const std::string_view key);
    std::string encrypt(const std::string_view decrypted_string) { return _encrypt(decrypted_string, true); }
    std::string decrypt(const std::string_view encrypted_string) { return _decrypt(encrypted_string, true); }
    std::string genUUID();
    bool setMachineUUID(std::string uuidstr);
    std::string getMachineUUID();

    void rsaSetPublicKey(const std::string_view n, const std::string_view e);
    void rsaSetPrivateKey(const std::string_view p, const std::string_view q, const std::string_view d);
    bool rsaEncrypt(unsigned char* msg, int size);
    bool rsaDecrypt(unsigned char* msg, int size);
    int rsaGetSize();

private:
    std::string _encrypt(const std::string_view decrypted_string, bool useMachineUUID);
    std::string _decrypt(const std::string_view encrypted_string, bool useMachineUUID);
    std::string getCryptKey(bool useMachineUUID);

    uuids::uuid m_machineUUID;

#ifdef USE_GMP
    mpz_t m_p, m_q, m_n, m_e, m_d;
#else
    RSA* m_rsa;
#endif
};

extern Crypt g_crypt;
