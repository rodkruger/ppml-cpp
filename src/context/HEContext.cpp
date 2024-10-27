//
// Created by rkruger on 23/10/24.
//

#include "context.h"

namespace hermesml {

    CryptoContext<DCRTPoly> HEContext::GetCc() {
        return this->cc;
    }

    void HEContext::SetCc(const CryptoContext<DCRTPoly>& cc) {
        this->cc = cc;
    }

    PublicKey<DCRTPoly> HEContext::GetPublicKey() {
        return this->publicKey;
    }

    void HEContext::SetPublicKey(const PublicKey<DCRTPoly>& publicKey) {
        this->publicKey = publicKey;
    }

    PrivateKey<DCRTPoly> HEContext::GetPrivateKey() {
        return this->privateKey;
    }

    void HEContext::SetPrivateKey(const PrivateKey<DCRTPoly>& privateKey) {
        this->privateKey = privateKey;
    }

    uint32_t HEContext::GetMultiplicativeDepth() const {
        return this->multiplicativeDepth;
    }

    void HEContext::SetMultiplicativeDepth(uint32_t multiplicativeDepth) {
        this->multiplicativeDepth = multiplicativeDepth;
    }

    uint32_t HEContext::GetNumSlots() const {
        return this->numSlots;
    }

    void HEContext::SetNumSlots(uint32_t numSlots) {
        this->numSlots = numSlots;
    }
}
