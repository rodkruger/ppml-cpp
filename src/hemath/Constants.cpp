#include "hemath.h"

namespace hermesml {
    Constants::Constants(const HEContext &ctx, int32_t n_features) : EncryptedObject(ctx), encryptedZero(nullptr),
                                                                     encryptedOne(nullptr),
                                                                     encrypted05(nullptr),
                                                                     encrypted125(nullptr),
                                                                     encrypted0625(nullptr) {
        this->encryptedZero = this->EncryptCKKS(std::vector<double>(n_features, 0));
        this->encryptedOne = this->EncryptCKKS(std::vector<double>(n_features, 1));
        this->encrypted05 = this->EncryptCKKS(std::vector<double>(n_features, 0.5));
        this->encrypted125 = this->EncryptCKKS(std::vector<double>(n_features, 0.125));
        this->encrypted0625 = this->EncryptCKKS(std::vector<double>(n_features, 0.0625));
    }

    BootstrapableCiphertext Constants::Zero() const {
        return this->encryptedZero;
    }

    BootstrapableCiphertext Constants::One() const {
        return this->encryptedOne;
    }

    BootstrapableCiphertext Constants::C05() const {
        return this->encrypted05;
    }

    BootstrapableCiphertext Constants::C125() const {
        return this->encrypted125;
    }

    BootstrapableCiphertext Constants::C0625() const {
        return this->encrypted0625;
    }
}
