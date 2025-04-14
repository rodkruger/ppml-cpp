#include "hemath.h"

namespace hermesml {
    Constants::Constants(const HEContext &ctx, const int32_t n_features) : EncryptedObject(ctx),
                                                                           n_features(n_features) {
        this->zero = this->EncryptCKKS(std::vector<double>(this->n_features, 0.0));
        this->one = this->EncryptCKKS(std::vector<double>(this->n_features, 1.0));
        this->two = this->EncryptCKKS(std::vector<double>(this->n_features, 2.0));
        this->c0_9580 = this->EncryptCKKS(std::vector<double>(n_features, 0.9580));
        this->c0_625 = this->EncryptCKKS(std::vector<double>(n_features, 0.0625));
        this->c0_5 = this->EncryptCKKS(std::vector<double>(n_features, 0.5));
        this->c0_333333 = this->EncryptCKKS(std::vector<double>(n_features, 0.333333));
        this->c0_25 = this->EncryptCKKS(std::vector<double>(n_features, 0.25));
        this->c0_21689 = this->EncryptCKKS(std::vector<double>(n_features, 0.21689));
        this->c0_2125 = this->EncryptCKKS(std::vector<double>(n_features, 0.2125));
        this->c0_133333 = this->EncryptCKKS(std::vector<double>(n_features, 0.133333));
        this->c0_125 = this->EncryptCKKS(std::vector<double>(n_features, 0.125));
        this->c0_0240 = this->EncryptCKKS(std::vector<double>(n_features, 0.0240));
        this->c0_020833 = this->EncryptCKKS(std::vector<double>(n_features, 0.020833));
        this->c0_002083 = this->EncryptCKKS(std::vector<double>(n_features, 0.002083));
        this->c0_0081934 = this->EncryptCKKS(std::vector<double>(n_features, 0.0081934));
        this->c0_00016588 = this->EncryptCKKS(std::vector<double>(n_features, 0.00016588));
        this->c0_0000011959 = this->EncryptCKKS(std::vector<double>(n_features, 0.0000011959));
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

    BootstrapableCiphertext Constants::C0_9580() const {
        return this->c0_9580;
    }

    BootstrapableCiphertext Constants::C0_625() const {
        return this->c0_625;
    }

    BootstrapableCiphertext Constants::C0_5() const {
        return this->c0_5;
    }

    BootstrapableCiphertext Constants::C0_333333() const {
        return this->c0_333333;
    }

    BootstrapableCiphertext Constants::C0_25() const {
        return this->c0_25;
    }

    BootstrapableCiphertext Constants::C0_21689() const {
        return this->c0_21689;
    }

    BootstrapableCiphertext Constants::C0_2125() const {
        return this->c0_2125;
    }

    BootstrapableCiphertext Constants::C0_125() const {
        return this->c0_125;
    }

    BootstrapableCiphertext Constants::C0_133333() const {
        return this->c0_133333;
    }

    BootstrapableCiphertext Constants::C0_0240() const {
        return this->c0_0240;
    }

    BootstrapableCiphertext Constants::C0_020833() const {
        return this->c0_020833;
    }

    BootstrapableCiphertext Constants::C0_002083() const {
        return this->c0_002083;
    }

    BootstrapableCiphertext Constants::C0_0081934() const {
        return this->c0_0081934;
    }

    BootstrapableCiphertext Constants::C0_00016588() const {
        return this->c0_00016588;
    }

    BootstrapableCiphertext Constants::C0_0000011959() const {
        return this->c0_0000011959;
    }
}
