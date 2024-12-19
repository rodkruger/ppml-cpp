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
        void SetMultiplicativeDepth(uint16_t multiplicativeDepth);

        uint32_t GetNumSlots();
        void SetNumSlots(uint16_t numSlots);

        uint32_t GetLevelsAfterBootstrapping();
        void SetLevelsAfterBootstrapping(uint16_t numSlots);

    private:
        CryptoContext<DCRTPoly> cc;
        PublicKey<DCRTPoly> publicKey;
        PrivateKey<DCRTPoly> privateKey;
        uint16_t multiplicativeDepth = 0;
        uint16_t numSlots = 0;
        uint16_t levelsAfterBootstrapping = 0;
    };

    class HEContextFactory
    {
    public:
        HEContext bgvHeContext();
        HEContext ckksHeContext();
    };
}

#endif //CONTEXT_H
