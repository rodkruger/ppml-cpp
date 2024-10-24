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

    public:
        explicit Constants(const HEContext &ctx);
        Ciphertext<DCRTPoly> Zero();
        Ciphertext<DCRTPoly> One();
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
