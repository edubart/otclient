#ifndef RSA_H
#define RSA_H

#include <framework/global.h>
#include <gmp.h>

class Rsa
{
public:
    Rsa();
    ~Rsa();
    void setKey(const char* p, const char* q, const char* d);
    bool setKey(const std::string& file);
    bool decrypt(char* msg, int32_t size);
    static bool encrypt(char* msg, int32_t size, const char* key);

protected:
    bool m_keySet;

    mpz_t m_p, m_q, m_u, m_d, m_dp, m_dq, m_mod;
};

typedef std::shared_ptr<Rsa> RsaPtr;

#endif //RSA_H