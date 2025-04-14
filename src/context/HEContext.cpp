//
// Created by rkruger on 23/10/24.
//

#include "context.h"

namespace hermesml {
    CryptoContext<DCRTPoly> HEContext::GetCc() const {
        return this->cc;
    }

    void HEContext::SetCc(const CryptoContext<DCRTPoly> &cc) {
        this->cc = cc;
    }

    PublicKey<DCRTPoly> HEContext::GetPublicKey() const {
        return this->publicKey;
    }

    void HEContext::SetPublicKey(const PublicKey<DCRTPoly> &publicKey) {
        this->publicKey = publicKey;
    }

    PrivateKey<DCRTPoly> HEContext::GetPrivateKey() const {
        return this->privateKey;
    }

    void HEContext::SetPrivateKey(const PrivateKey<DCRTPoly> &privateKey) {
        this->privateKey = privateKey;
    }

    int32_t HEContext::GetScalingModSize() const {
        return this->scalingModSize;
    }

    void HEContext::SetScalingModSize(const int32_t scalingModSize) {
        this->scalingModSize = scalingModSize;
    }

    int32_t HEContext::GetMultiplicativeDepth() const {
        return this->multiplicativeDepth;
    }

    void HEContext::SetMultiplicativeDepth(const int32_t multiplicativeDepth) {
        this->multiplicativeDepth = multiplicativeDepth;
    }

    uint32_t HEContext::GetNumSlots() const {
        return this->numSlots;
    }

    void HEContext::SetNumSlots(const uint32_t numSlots) {
        this->numSlots = numSlots;
    }

    uint32_t HEContext::GetLevelsAfterBootstrapping() const {
        return this->levelsAfterBootstrapping;
    }

    void HEContext::SetLevelsAfterBootstrapping(const uint32_t levelsAfterBootstrapping) {
        this->levelsAfterBootstrapping = levelsAfterBootstrapping;
    }

    int32_t HEContext::GetEarlyBootstrapping() const {
        return this->earlyBootstrapping;
    }

    void HEContext::SetEarlyBootstrapping(const int32_t earlyBootstrapping) {
        this->earlyBootstrapping = earlyBootstrapping;
    }
}
