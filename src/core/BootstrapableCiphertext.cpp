#include "core.h"

namespace hermesml {
    BootstrapableCiphertext::BootstrapableCiphertext() = default;

    BootstrapableCiphertext::BootstrapableCiphertext(const Ciphertext<DCRTPoly> &ciphertext,
                                                     const int8_t remainingLevels,
                                                     const int32_t additionsExecuted) : ciphertext(ciphertext),
        remainingLevels(remainingLevels), additionsExecuted(additionsExecuted) {
        if (remainingLevels <= 0 || remainingLevels > 30) {
            std::cout << "Warning: remainingLevels must be between 0 and 30!" << std::endl;
        }
    }

    int8_t BootstrapableCiphertext::GetRemainingLevels() const {
        return this->remainingLevels;
    }

    void BootstrapableCiphertext::SetRemainingLevels(const int8_t pRemainingLevels) {
        this->remainingLevels = pRemainingLevels;

        if (remainingLevels <= 0 || remainingLevels > 30) {
            std::cout << "Warning: remainingLevels must be between 0 and 30!" << std::endl;
        }
    }

    int32_t BootstrapableCiphertext::GetAdditionsExecuted() const {
        return this->additionsExecuted;
    }

    void BootstrapableCiphertext::SetRemainingLevels(const int32_t additionsExecuted) {
        this->additionsExecuted = additionsExecuted;
    }

    Ciphertext<DCRTPoly> BootstrapableCiphertext::GetCiphertext() const {
        return this->ciphertext;
    }
}
