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

#ifndef CRYPT_H
#define CRYPT_H

#include "../stdext/types.h"
#include <string>

#include <boost/uuid/uuid.hpp>

#ifdef USE_GMP

#include <gmp.h>

#else

typedef struct rsa_st RSA;

#endif

class Crypt
{
public:
    Crypt();
    ~Crypt();

    std::string base64Encode(const std::string& decoded_string);
    std::string base64Decode(const std::string& encoded_string);
    std::string xorCrypt(const std::string& buffer, const std::string& key);
    std::string encrypt(const std::string& decrypted_string) { return _encrypt(decrypted_string, true); }
    std::string decrypt(const std::string& encrypted_string) { return _decrypt(encrypted_string, true); }
    std::string genUUID();
    bool setMachineUUID(std::string uuidstr);
    std::string getMachineUUID();

#ifndef USE_GMP

    std::string md5Encode(const std::string& decoded_string, bool upperCase);
    std::string sha1Encode(const std::string& decoded_string, bool upperCase);
    std::string sha256Encode(const std::string& decoded_string, bool upperCase);
    std::string sha512Encode(const std::string& decoded_string, bool upperCase);

    void rsaGenerateKey(int bits, int e);

#endif

    void rsaSetPublicKey(const std::string& n, const std::string& e);
    void rsaSetPrivateKey(const std::string &p, const std::string &q, const std::string &d);

#ifdef USE_GMP

    bool rsaEncrypt(char *msg, int size);
    bool rsaDecrypt(char *msg, int size);

#else

    bool rsaCheckKey();
    bool rsaEncrypt(unsigned char *msg, int size);
    bool rsaDecrypt(unsigned char *msg, int size);

#endif
    int rsaGetSize();

#ifdef USE_GMP

    static constexpr auto MODULUS_SIZE = 1024;
    static constexpr auto BLOCK_SIZE = MODULUS_SIZE / 8;

#endif

private:
    std::string _encrypt(const std::string& decrypted_string, bool useMachineUUID);
    std::string _decrypt(const std::string& encrypted_string, bool useMachineUUID);
    std::string getCryptKey(bool useMachineUUID);
    boost::uuids::uuid m_machineUUID;
#ifdef USE_GMP

    mpz_t rsaN, rsaE, rsaD;

#else

    RSA *m_rsa;

#endif
};

extern Crypt g_crypt;

#endif
