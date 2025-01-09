#include "core.h"

namespace hermesml {
    BootstrapableCiphertext::BootstrapableCiphertext() = default;

    BootstrapableCiphertext::BootstrapableCiphertext(const Ciphertext<DCRTPoly> &ciphertext,
                                                     const uint8_t remainingLevels) {
        this->ciphertext = ciphertext;
        this->remainingLevels = remainingLevels;
    }

    uint8_t BootstrapableCiphertext::GetRemainingLevels() const {
        return this->remainingLevels;
    }

    void BootstrapableCiphertext::SetRemainingLevels(uint8_t pRemainingLevels) {
        this->remainingLevels = pRemainingLevels;
    }

    Ciphertext<DCRTPoly> BootstrapableCiphertext::GetCiphertext() const {
        return this->ciphertext;
    }
}
