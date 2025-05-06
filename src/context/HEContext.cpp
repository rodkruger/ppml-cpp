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

    uint32_t HEContext::GetScalingModSize() const {
        return this->scalingModSize;
    }

    void HEContext::SetScalingModSize(const uint32_t scalingModSize) {
        this->scalingModSize = scalingModSize;
    }

    uint32_t HEContext::GetMultiplicativeDepth() const {
        return this->multiplicativeDepth;
    }

    void HEContext::SetMultiplicativeDepth(const uint32_t multiplicativeDepth) {
        this->multiplicativeDepth = multiplicativeDepth;
    }

    uint32_t HEContext::GetNumSlots() const {
        return this->numSlots;
    }

    void HEContext::SetNumSlots(const uint32_t numSlots) {
        this->numSlots = numSlots;
    }

    int32_t HEContext::GetLevelsAfterBootstrapping() const {
        return this->levelsAfterBootstrapping;
    }

    void HEContext::SetLevelsAfterBootstrapping(const int32_t levelsAfterBootstrapping) {
        this->levelsAfterBootstrapping = levelsAfterBootstrapping;
    }

    int32_t HEContext::GetEarlyBootstrapping() const {
        return this->earlyBootstrapping;
    }

    void HEContext::SetEarlyBootstrapping(const int32_t earlyBootstrapping) {
        this->earlyBootstrapping = earlyBootstrapping;
    }

    uint32_t HEContext::GetNumFeatures() const {
        return this->numFeatures;
    }

    void HEContext::SetNumFeatures(const uint32_t numFeatures) {
        this->numFeatures = numFeatures;
    }
}
