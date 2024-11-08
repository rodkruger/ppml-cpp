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
        Ciphertext<DCRTPoly> encryptedZero;
        Ciphertext<DCRTPoly> encryptedOne;
        Ciphertext<DCRTPoly> encrypted05;
        Ciphertext<DCRTPoly> encrypted125;
        Ciphertext<DCRTPoly> encrypted0625;

    public:
        explicit Constants(const HEContext &ctx, int32_t n_features);
        [[nodiscard]] Ciphertext<DCRTPoly> Zero() const;
        [[nodiscard]] Ciphertext<DCRTPoly> One() const;
        [[nodiscard]] Ciphertext<DCRTPoly> C05() const;
        [[nodiscard]] Ciphertext<DCRTPoly> C125() const;
        [[nodiscard]] Ciphertext<DCRTPoly> C0625() const;
    };

    class Calculus : EncryptedObject {
    private:
        Constants constants;

    public:
        explicit Calculus(const HEContext &ctx);
        Ciphertext<DCRTPoly> TaylorSqrt(const Ciphertext<DCRTPoly>& x);
        Ciphertext<DCRTPoly> Euclidean(const Ciphertext<DCRTPoly>& point1,
                                       const Ciphertext<DCRTPoly>& point2);
    };

    class CalculusQuant : EncryptedObject {
    private:
        Constants constants;

    public:
        explicit CalculusQuant(const HEContext &ctx);
        Ciphertext<DCRTPoly> TaylorSqrt(const Ciphertext<DCRTPoly>& x);
        Ciphertext<DCRTPoly> Euclidean(const Ciphertext<DCRTPoly>& point1,
                                       const Ciphertext<DCRTPoly>& point2);
    };


}

#endif //HEMATH_H
