#include "hemath.h"

namespace hermesml {
    Constants::Constants(const HEContext &ctx, const int32_t n_features) : EncryptedObject(ctx) {
        this->encryptedZero = this->EncryptCKKS(std::vector<double>(n_features, 0));
        this->encryptedOne = this->EncryptCKKS(std::vector<double>(n_features, 1));
        this->encrypted05 = this->EncryptCKKS(std::vector<double>(n_features, 0.5));
        this->encrypted125 = this->EncryptCKKS(std::vector<double>(n_features, 0.125));
        this->encrypted0625 = this->EncryptCKKS(std::vector<double>(n_features, 0.0625));
        this->encrypted0333333 = this->EncryptCKKS(std::vector<double>(n_features, 0.333333));
        this->encrypted0133333 = this->EncryptCKKS(std::vector<double>(n_features, 0.133333));

        this->encrypted021689 = this->EncryptCKKS(std::vector<double>(n_features, 0.21689));
        this->encrypted00081934 = this->EncryptCKKS(std::vector<double>(n_features, 0.0081934));
        this->encrypted000016588 = this->EncryptCKKS(std::vector<double>(n_features, 0.00016588));
        this->encrypted00000011959 = this->EncryptCKKS(std::vector<double>(n_features, 0.0000011959));
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

    BootstrapableCiphertext Constants::C0333333() const {
        return this->encrypted0333333;
    }

    BootstrapableCiphertext Constants::C0133333() const {
        return this->encrypted0133333;
    }

    BootstrapableCiphertext Constants::C021689() const {
        return this->encrypted021689;
    }

    BootstrapableCiphertext Constants::C00081934() const {
        return this->encrypted00081934;
    }

    BootstrapableCiphertext Constants::C000016588() const {
        return this->encrypted000016588;
    }

    BootstrapableCiphertext Constants::C00000011959() const {
        return this->encrypted00000011959;
    }
}
