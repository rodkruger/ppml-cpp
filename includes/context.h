#ifndef CONTEXT_H
#define CONTEXT_H

#include "openfhe.h"

using namespace lbcrypto;

namespace hermesml {

    class HEContext {
    public:
        CryptoContext<DCRTPoly> GetCc();
        void SetCc(const CryptoContext<DCRTPoly>& cc);

        PublicKey<DCRTPoly> GetPublicKey();
        void SetPublicKey(const PublicKey<DCRTPoly>& publicKey);

        uint32_t GetMultiplicativeDepth() const;
        void SetMultiplicativeDepth(uint32_t multiplicativeDepth);

        uint32_t GetNumSlots() const;
        void SetNumSlots(uint32_t numSlots);

    private:
        CryptoContext<DCRTPoly> cc;
        PublicKey<DCRTPoly> publicKey;
        uint32_t multiplicativeDepth = 0;
        uint32_t numSlots = 0;

    };

    class HEContextFactory {
    public:
        HEContext bgvHeContext();
        HEContext ckksHeContext();
    };
}

#endif //CONTEXT_H
