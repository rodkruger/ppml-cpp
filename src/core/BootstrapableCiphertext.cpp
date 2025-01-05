#include "core.h"

namespace hermesml {
    BootstrapableCiphertext::BootstrapableCiphertext() = default;

    BootstrapableCiphertext::BootstrapableCiphertext(const Ciphertext<DCRTPoly> &ciphertext) {
        this->ciphertext = ciphertext;
    }

    void BootstrapableCiphertext::DecrementLevel() {
        this->remainingLevels--;
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
