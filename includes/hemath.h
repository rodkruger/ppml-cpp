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
    private:
        BootstrapableCiphertext encryptedZero;
        BootstrapableCiphertext encryptedOne;
        BootstrapableCiphertext encrypted05;
        BootstrapableCiphertext encrypted125;
        BootstrapableCiphertext encrypted0625;
        BootstrapableCiphertext encrypted0333333;
        BootstrapableCiphertext encrypted0133333;

    public:
        explicit Constants(const HEContext &ctx, int32_t n_features);

        [[nodiscard]] BootstrapableCiphertext Zero() const;

        [[nodiscard]] BootstrapableCiphertext One() const;

        [[nodiscard]] BootstrapableCiphertext C05() const;

        [[nodiscard]] BootstrapableCiphertext C125() const;

        [[nodiscard]] BootstrapableCiphertext C0625() const;

        [[nodiscard]] BootstrapableCiphertext C0333333() const;

        [[nodiscard]] BootstrapableCiphertext C0133333() const;
    };

    class Calculus : EncryptedObject {
    private:
        Constants ants;

    public:
        explicit Calculus(const HEContext &ctx);

        [[nodiscard]] BootstrapableCiphertext TaylorSqrt(const BootstrapableCiphertext &x) const;

        // [[nodiscard]] BootstrapableCiphertext Euclidean(const BootstrapableCiphertext &point1,
        //                                                 const BootstrapableCiphertext &point2) const;
    };

    class CalculusQuant : EncryptedObject {
    private:
        Constants constants;

    public:
        explicit CalculusQuant(const HEContext &ctx);

        [[nodiscard]] BootstrapableCiphertext TaylorSqrt(const BootstrapableCiphertext &x) const;

        // [[nodiscard]] BootstrapableCiphertext Euclidean(const BootstrapableCiphertext &point1,
        //                                                 const BootstrapableCiphertext &point2) const;
    };
}

#endif //HEMATH_H
