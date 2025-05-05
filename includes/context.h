#ifndef CONTEXT_H
#define CONTEXT_H

#include "openfhe.h"

using namespace lbcrypto;

namespace hermesml {
    class HEContext {
        CryptoContext<DCRTPoly> cc;
        PublicKey<DCRTPoly> publicKey;
        PrivateKey<DCRTPoly> privateKey;
        uint32_t scalingModSize = 0;
        uint32_t multiplicativeDepth = 0;
        uint32_t numSlots = 0;
        uint32_t levelsAfterBootstrapping = 0;
        uint32_t earlyBootstrapping = 0;
        uint32_t numFeatures = 0;

    public:
        [[nodiscard]] CryptoContext<DCRTPoly> GetCc() const;

        void SetCc(const CryptoContext<DCRTPoly> &cc);

        [[nodiscard]] PublicKey<DCRTPoly> GetPublicKey() const;

        void SetPublicKey(const PublicKey<DCRTPoly> &publicKey);

        [[nodiscard]] PrivateKey<DCRTPoly> GetPrivateKey() const;

        void SetPrivateKey(const PrivateKey<DCRTPoly> &privateKey);

        [[nodiscard]] uint32_t GetScalingModSize() const;

        void SetScalingModSize(uint32_t scalingModSize);

        [[nodiscard]] uint32_t GetMultiplicativeDepth() const;

        void SetMultiplicativeDepth(uint32_t multiplicativeDepth);

        [[nodiscard]] uint32_t GetNumSlots() const;

        void SetNumSlots(uint32_t numSlots);

        [[nodiscard]] uint32_t GetLevelsAfterBootstrapping() const;

        void SetLevelsAfterBootstrapping(uint32_t levelsAfterBootstrapping);

        [[nodiscard]] uint32_t GetEarlyBootstrapping() const;

        void SetEarlyBootstrapping(uint32_t earlyBootstrapping);

        [[nodiscard]] uint32_t GetNumFeatures() const;

        void SetNumFeatures(uint32_t numFeatures);
    };

    class HEContextFactory {
    private:
        [[nodiscard]] static uint32_t NextPowerOfTwo(uint32_t n);

    public:
        [[nodiscard]] static HEContext ckksHeContext(uint32_t n_features);
    };
}

#endif //CONTEXT_H
