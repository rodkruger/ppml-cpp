#ifndef CONTEXT_H
#define CONTEXT_H

#include "openfhe.h"

using namespace lbcrypto;

namespace hermesml {
    class HEContext {
        CryptoContext<DCRTPoly> cc;
        PublicKey<DCRTPoly> publicKey;
        PrivateKey<DCRTPoly> privateKey;
        uint16_t multiplicativeDepth = 0;
        uint16_t numSlots = 0;
        uint16_t levelsAfterBootstrapping = 0;

    public:
        [[nodiscard]] CryptoContext<DCRTPoly> GetCc() const;

        void SetCc(const CryptoContext<DCRTPoly> &cc);

        [[nodiscard]] PublicKey<DCRTPoly> GetPublicKey() const;

        void SetPublicKey(const PublicKey<DCRTPoly> &publicKey);

        [[nodiscard]] PrivateKey<DCRTPoly> GetPrivateKey() const;

        void SetPrivateKey(const PrivateKey<DCRTPoly> &privateKey);

        [[nodiscard]] uint16_t GetMultiplicativeDepth() const;

        void SetMultiplicativeDepth(uint16_t multiplicativeDepth);

        [[nodiscard]] uint16_t GetNumSlots() const;

        void SetNumSlots(uint16_t numSlots);

        [[nodiscard]] uint32_t GetLevelsAfterBootstrapping() const;

        void SetLevelsAfterBootstrapping(uint16_t numSlots);
    };

    class HEContextFactory {
    public:
        [[nodiscard]] static HEContext bgvHeContext();

        [[nodiscard]] static HEContext ckksHeContext();
    };
}

#endif //CONTEXT_H
