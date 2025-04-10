//
// Created by rkruger on 23/10/24.
//

#ifndef HEMATH_H
#define HEMATH_H

#include "openfhe.h"
#include "context.h"
#include "core.h"

#define TAYLOR_SQRT_PRECISION 3

using namespace lbcrypto;

namespace hermesml {
    class Constants : EncryptedObject {
        int32_t n_features;
        BootstrapableCiphertext zero;
        BootstrapableCiphertext one;
        BootstrapableCiphertext two;
        BootstrapableCiphertext c05;
        BootstrapableCiphertext c125;
        BootstrapableCiphertext c0625;
        BootstrapableCiphertext c0333333;
        BootstrapableCiphertext c0133333;
        BootstrapableCiphertext c021689;
        BootstrapableCiphertext c00081934;
        BootstrapableCiphertext c000016588;
        BootstrapableCiphertext c00000011959;

    public:
        explicit Constants(const HEContext &ctx, int32_t n_features);

        [[nodiscard]] BootstrapableCiphertext Zero() const;

        [[nodiscard]] BootstrapableCiphertext One() const;

        [[nodiscard]] BootstrapableCiphertext Two() const;

        [[nodiscard]] BootstrapableCiphertext C05() const;

        [[nodiscard]] BootstrapableCiphertext C125() const;

        [[nodiscard]] BootstrapableCiphertext C0625() const;

        [[nodiscard]] BootstrapableCiphertext C0333333() const;

        [[nodiscard]] BootstrapableCiphertext C0133333() const;

        [[nodiscard]] BootstrapableCiphertext C021689() const;

        [[nodiscard]] BootstrapableCiphertext C00081934() const;

        [[nodiscard]] BootstrapableCiphertext C000016588() const;

        [[nodiscard]] BootstrapableCiphertext C00000011959() const;
    };

    class Calculus : EncryptedObject {
    private:
        Constants ants;

    public:
        explicit Calculus(const HEContext &ctx);

        [[nodiscard]] BootstrapableCiphertext TaylorSqrt(const BootstrapableCiphertext &x) const;
    };

    class CalculusQuant : EncryptedObject {
        Constants constants;

    public:
        explicit CalculusQuant(const HEContext &ctx);

        [[nodiscard]] BootstrapableCiphertext TaylorSqrt(const BootstrapableCiphertext &x) const;
    };
}

#endif //HEMATH_H
