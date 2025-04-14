#include "core.h"

namespace hermesml {
    BootstrapableCiphertext::BootstrapableCiphertext() = default;

    BootstrapableCiphertext::BootstrapableCiphertext(const Ciphertext<DCRTPoly> &ciphertext,
                                                     const int32_t remainingLevels,
                                                     const int32_t additionsExecuted) : ciphertext(ciphertext),
        remainingLevels(remainingLevels), additionsExecuted(additionsExecuted) {
    }

    int32_t BootstrapableCiphertext::GetRemainingLevels() const {
        return this->remainingLevels;
    }

    void BootstrapableCiphertext::SetRemainingLevels(const int32_t pRemainingLevels) {
        this->remainingLevels = pRemainingLevels;
    }

    int32_t BootstrapableCiphertext::GetAdditionsExecuted() const {
        return this->additionsExecuted;
    }

    const Ciphertext<DCRTPoly> &BootstrapableCiphertext::GetCiphertext() const {
        return this->ciphertext;
    }
}
