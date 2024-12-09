//
// Created by rkruger on 23/10/24.
//

#include "context.h"

namespace hermesml
{
    CryptoContext<DCRTPoly> HEContext::GetCc()
    {
        return this->cc;
    }

    void HEContext::SetCc(CryptoContext<DCRTPoly> cc)
    {
        this->cc = cc;
    }

    PublicKey<DCRTPoly> HEContext::GetPublicKey()
    {
        return this->publicKey;
    }

    void HEContext::SetPublicKey(PublicKey<DCRTPoly> publicKey)
    {
        this->publicKey = publicKey;
    }

    PrivateKey<DCRTPoly> HEContext::GetPrivateKey()
    {
        return this->privateKey;
    }

    void HEContext::SetPrivateKey(PrivateKey<DCRTPoly> privateKey)
    {
        this->privateKey = privateKey;
    }

    uint32_t HEContext::GetMultiplicativeDepth()
    {
        return this->multiplicativeDepth;
    }

    void HEContext::SetMultiplicativeDepth(uint32_t multiplicativeDepth)
    {
        this->multiplicativeDepth = multiplicativeDepth;
    }

    uint32_t HEContext::GetNumSlots()
    {
        return this->numSlots;
    }

    void HEContext::SetNumSlots(uint32_t numSlots)
    {
        this->numSlots = numSlots;
    }
}
