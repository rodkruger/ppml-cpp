#include "hemath.h"

namespace hermesml {
    Constants::Constants(const HEContext &ctx, const int32_t n_features) : EncryptedObject(ctx),
                                                                           n_features(n_features) {
        this->zero = this->EncryptCKKS(std::vector<double>(this->n_features, 0.0));
        this->one = this->EncryptCKKS(std::vector<double>(this->n_features, 1.0));
        this->two = this->EncryptCKKS(std::vector<double>(this->n_features, 2.0));
        this->c05 = this->EncryptCKKS(std::vector<double>(n_features, 0.5));
        this->c125 = this->EncryptCKKS(std::vector<double>(n_features, 0.125));
        this->c0625 = this->EncryptCKKS(std::vector<double>(n_features, 0.0625));
        this->c0333333 = this->EncryptCKKS(std::vector<double>(n_features, 0.333333));
        this->c0133333 = this->EncryptCKKS(std::vector<double>(n_features, 0.133333));
        this->c021689 = this->EncryptCKKS(std::vector<double>(n_features, 0.21689));
        this->c00081934 = this->EncryptCKKS(std::vector<double>(n_features, 0.0081934));
        this->c000016588 = this->EncryptCKKS(std::vector<double>(n_features, 0.00016588));
        this->c00000011959 = this->EncryptCKKS(std::vector<double>(n_features, 0.0000011959));
    }

    BootstrapableCiphertext Constants::Zero() const {
        return this->zero;
    }

    BootstrapableCiphertext Constants::One() const {
        return this->one;
    }

    BootstrapableCiphertext Constants::Two() const {
        return this->two;
    }

    BootstrapableCiphertext Constants::C05() const {
        return this->c05;
    }

    BootstrapableCiphertext Constants::C125() const {
        return this->c125;
    }

    BootstrapableCiphertext Constants::C0625() const {
        return this->c0625;
    }

    BootstrapableCiphertext Constants::C0333333() const {
        return this->c0333333;
    }

    BootstrapableCiphertext Constants::C0133333() const {
        return this->c0133333;
    }

    BootstrapableCiphertext Constants::C021689() const {
        return this->c021689;
    }

    BootstrapableCiphertext Constants::C00081934() const {
        return this->c00081934;
    }

    BootstrapableCiphertext Constants::C000016588() const {
        return this->c000016588;
    }

    BootstrapableCiphertext Constants::C00000011959() const {
        return this->c00000011959;
    }
}
