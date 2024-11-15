#ifndef CONTEXT_H
#define CONTEXT_H

#include "openfhe.h"

using namespace lbcrypto;

namespace hermesml
{
    class HEContext
    {
    public:
        CryptoContext<DCRTPoly> GetCc();
        void SetCc(CryptoContext<DCRTPoly> cc);

        PublicKey<DCRTPoly> GetPublicKey();
        void SetPublicKey(PublicKey<DCRTPoly> publicKey);

        PrivateKey<DCRTPoly> GetPrivateKey();
        void SetPrivateKey(PrivateKey<DCRTPoly> privateKey);

        uint32_t GetMultiplicativeDepth();
        void SetMultiplicativeDepth(uint32_t multiplicativeDepth);

        uint32_t GetNumSlots();
        void SetNumSlots(uint32_t numSlots);

    private:
        CryptoContext<DCRTPoly> cc;
        PublicKey<DCRTPoly> publicKey;
        PrivateKey<DCRTPoly> privateKey;
        uint32_t multiplicativeDepth = 0;
        uint32_t numSlots = 0;
    };

    class HEContextFactory
    {
    public:
        HEContext bgvHeContext();
        HEContext ckksHeContext();
    };
}

#endif //CONTEXT_H
