//
// Created by rkruger on 23/10/24.
//

#ifndef HEMATH_H
#define HEMATH_H

#include "openfhe.h"
#include "context.h"
#include "core.h"

using namespace lbcrypto;

namespace hermesml {
    enum ActivationFn { TANH, SIGMOID };

    enum ApproximationFn { CHEBYSHEV, TAYLOR, LEAST_SQUARES };

    class Constants : EncryptedObject {
        BootstrapableCiphertext zero;
        BootstrapableCiphertext one;
        BootstrapableCiphertext two;
        BootstrapableCiphertext c0_9580;
        BootstrapableCiphertext c0_625;
        BootstrapableCiphertext c0_5;
        BootstrapableCiphertext c0_333333;
        BootstrapableCiphertext c0_25;
        BootstrapableCiphertext c0_2125;
        BootstrapableCiphertext c0_133333;
        BootstrapableCiphertext c0_125;
        BootstrapableCiphertext c0_21689;
        BootstrapableCiphertext c0_0240;
        BootstrapableCiphertext c0_020833;
        BootstrapableCiphertext c0_002083;
        BootstrapableCiphertext c0_0081934;
        BootstrapableCiphertext c0_00016588;
        BootstrapableCiphertext c0_0000011959;

    public:
        explicit Constants(const HEContext &ctx);

        [[nodiscard]] BootstrapableCiphertext Zero() const;

        [[nodiscard]] BootstrapableCiphertext One() const;

        [[nodiscard]] BootstrapableCiphertext Two() const;

        [[nodiscard]] BootstrapableCiphertext C0_9580() const;

        [[nodiscard]] BootstrapableCiphertext C0_625() const;

        [[nodiscard]] BootstrapableCiphertext C0_5() const;

        [[nodiscard]] BootstrapableCiphertext C0_333333() const;

        [[nodiscard]] BootstrapableCiphertext C0_25() const;

        [[nodiscard]] BootstrapableCiphertext C0_21689() const;

        [[nodiscard]] BootstrapableCiphertext C0_2125() const;

        [[nodiscard]] BootstrapableCiphertext C0_133333() const;

        [[nodiscard]] BootstrapableCiphertext C0_125() const;

        [[nodiscard]] BootstrapableCiphertext C0_0240() const;

        [[nodiscard]] BootstrapableCiphertext C0_020833() const;

        [[nodiscard]] BootstrapableCiphertext C0_002083() const;

        [[nodiscard]] BootstrapableCiphertext C0_0081934() const;

        [[nodiscard]] BootstrapableCiphertext C0_00016588() const;

        [[nodiscard]] BootstrapableCiphertext C0_0000011959() const;
    };

    class Calculus : EncryptedObject {
        Constants constants;

        [[nodiscard]] BootstrapableCiphertext SigmoidTaylor(const BootstrapableCiphertext &x) const;

        [[nodiscard]] BootstrapableCiphertext SigmoidLeastSquares(const BootstrapableCiphertext &x) const;

        [[nodiscard]] BootstrapableCiphertext SigmoidChebyshev(const BootstrapableCiphertext &x) const;

        [[nodiscard]] BootstrapableCiphertext TanhTaylor(const BootstrapableCiphertext &x) const;

        [[nodiscard]] BootstrapableCiphertext TanhLeastSquares(const BootstrapableCiphertext &x) const;

        [[nodiscard]] BootstrapableCiphertext TanhChebyshev(const BootstrapableCiphertext &x) const;

    public:
        explicit Calculus(const HEContext &ctx);

        [[nodiscard]] BootstrapableCiphertext Sigmoid(const BootstrapableCiphertext &x,
                                                      ApproximationFn approximation) const;

        [[nodiscard]] BootstrapableCiphertext SigmoidDerivative(const BootstrapableCiphertext &x,
                                                                ApproximationFn approximation) const;

        [[nodiscard]] BootstrapableCiphertext Tanh(const BootstrapableCiphertext &x,
                                                   ApproximationFn approximation) const;

        [[nodiscard]] BootstrapableCiphertext TanhDerivative(const BootstrapableCiphertext &x,
                                                             ApproximationFn approximation) const;

        [[nodiscard]] static std::vector<std::vector<double> > Transpose(const std::vector<std::vector<double> > &mat);
    };

    class CalculusQuant : EncryptedObject {
        Constants constants;

    public:
        explicit CalculusQuant(const HEContext &ctx);

        [[nodiscard]] BootstrapableCiphertext TaylorSqrt(const BootstrapableCiphertext &x) const;
    };
}

#endif //HEMATH_H
