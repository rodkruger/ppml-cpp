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

    uint16_t HEContext::GetScalingModSize() const {
        return this->scalingModSize;
    }

    void HEContext::SetScalingModSize(const uint16_t scalingModSize) {
        this->scalingModSize = scalingModSize;
    }

    uint16_t HEContext::GetMultiplicativeDepth() const {
        return this->multiplicativeDepth;
    }

    void HEContext::SetMultiplicativeDepth(const uint16_t multiplicativeDepth) {
        this->multiplicativeDepth = multiplicativeDepth;
    }

    uint16_t HEContext::GetNumSlots() const {
        return this->numSlots;
    }

    void HEContext::SetNumSlots(const uint16_t numSlots) {
        this->numSlots = numSlots;
    }

    uint32_t HEContext::GetLevelsAfterBootstrapping() const {
        return this->levelsAfterBootstrapping;
    }

    void HEContext::SetLevelsAfterBootstrapping(const uint16_t levelsAfterBootstrapping) {
        this->levelsAfterBootstrapping = levelsAfterBootstrapping;
    }
}
