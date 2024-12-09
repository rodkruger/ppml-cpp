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

namespace hermesml
{
    class Constants : EncryptedObject
    {
    private:
        Ciphertext<DCRTPoly> encryptedZero;
        Ciphertext<DCRTPoly> encryptedOne;
        Ciphertext<DCRTPoly> encrypted05;
        Ciphertext<DCRTPoly> encrypted125;
        Ciphertext<DCRTPoly> encrypted0625;

    public:
        explicit Constants(HEContext ctx, int32_t n_features);
        [[nodiscard]] Ciphertext<DCRTPoly> Zero();
        [[nodiscard]] Ciphertext<DCRTPoly> One();
        [[nodiscard]] Ciphertext<DCRTPoly> C05();
        [[nodiscard]] Ciphertext<DCRTPoly> C125();
        [[nodiscard]] Ciphertext<DCRTPoly> C0625();
    };

    class Calculus : EncryptedObject
    {
    private:
        Constants ants;

    public:
        explicit Calculus(HEContext ctx);
        Ciphertext<DCRTPoly> TaylorSqrt(Ciphertext<DCRTPoly> x);
        Ciphertext<DCRTPoly> Euclidean(Ciphertext<DCRTPoly> point1,
                                       Ciphertext<DCRTPoly> point2);
    };

    class CalculusQuant : EncryptedObject
    {
    private:
        Constants ants;

    public:
        explicit CalculusQuant(HEContext ctx);
        Ciphertext<DCRTPoly> TaylorSqrt(Ciphertext<DCRTPoly> x);
        Ciphertext<DCRTPoly> Euclidean(Ciphertext<DCRTPoly> point1,
                                       Ciphertext<DCRTPoly> point2);
    };
}

#endif //HEMATH_H
