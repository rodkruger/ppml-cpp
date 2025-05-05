#include "hemath.h"

namespace hermesml {
    Calculus::Calculus(const HEContext &ctx) : EncryptedObject(ctx), constants(Constants(ctx)) {
    }

    BootstrapableCiphertext Calculus::SigmoidChebyshev(const BootstrapableCiphertext &x) const {
        constexpr auto levels = 4;
        const auto decLevels = x.GetRemainingLevels() - levels;

        const auto b = this->
                EvalBootstrap(BootstrapableCiphertext(x.GetCiphertext(), static_cast<int8_t>(decLevels),
                                                      x.GetAdditionsExecuted()));

        auto c = this->GetCc()->EvalLogistic(b.GetCiphertext(), -6.0, 6.0, 5);
        c = this->SafeRescaling(c);

        return BootstrapableCiphertext(c, b.GetRemainingLevels(), b.GetAdditionsExecuted());
    }

    BootstrapableCiphertext Calculus::SigmoidTaylor(const BootstrapableCiphertext &x) const {
        const int decLevels = x.GetRemainingLevels() - 4;
        const auto b = this->EvalBootstrap(BootstrapableCiphertext(x.GetCiphertext(), static_cast<int8_t>(decLevels),
                                                                   x.GetAdditionsExecuted()));

        const std::vector<double> coefficients = {
            0.5,
            0.25,
            0.0,
            -0.0208333333333,
            0,
            0.00208333333333
        };

        auto c = this->GetCc()->EvalPolyLinear(b.GetCiphertext(), coefficients);
        c = this->SafeRescaling(c);
        return BootstrapableCiphertext(c, b.GetRemainingLevels(), b.GetAdditionsExecuted());
    }

    BootstrapableCiphertext Calculus::SigmoidLeastSquares(const BootstrapableCiphertext &x) const {
        const int decLevels = x.GetRemainingLevels() - 4;
        const auto b = this->EvalBootstrap(BootstrapableCiphertext(x.GetCiphertext(), static_cast<int8_t>(decLevels),
                                                                   x.GetAdditionsExecuted()));

        const std::vector<double> coefficients = {
            0.5,
            0.21703267840379,
            0.0,
            -0.007811149394003,
            0.0,
            0.000117906071735
        };

        auto c = this->GetCc()->EvalPolyLinear(b.GetCiphertext(), coefficients);
        c = this->SafeRescaling(c);
        return BootstrapableCiphertext(c, b.GetRemainingLevels(), b.GetAdditionsExecuted());
    }

    BootstrapableCiphertext Calculus::TanhChebyshev(const BootstrapableCiphertext &x) const {
        const int decLevels = x.GetRemainingLevels() - 7;

        const auto b = this->
                EvalBootstrap(BootstrapableCiphertext(x.GetCiphertext(), static_cast<int8_t>(decLevels),
                                                      x.GetAdditionsExecuted()));

        auto c = this->GetCc()->EvalChebyshevFunction(
            [](const double x1) { return tanh(x1); }, b.GetCiphertext(),
            -6, 6,
            59);
        c = this->SafeRescaling(c);

        return BootstrapableCiphertext(c, b.GetRemainingLevels(), b.GetAdditionsExecuted());
    }

    BootstrapableCiphertext Calculus::TanhTaylor(const BootstrapableCiphertext &x) const {
        const int decLevels = x.GetRemainingLevels() - 4;
        const auto b = this->EvalBootstrap(BootstrapableCiphertext(x.GetCiphertext(), static_cast<int8_t>(decLevels),
                                                                   x.GetAdditionsExecuted()));

        const std::vector<double> coefficients = {
            0.0,
            1.0,
            0.0,
            -0.3333333333333333,
            0.0,
            0.1333333333333333,
        };

        auto c = this->GetCc()->EvalPolyLinear(b.GetCiphertext(), coefficients);
        c = this->SafeRescaling(c);
        return BootstrapableCiphertext(c, b.GetRemainingLevels(), b.GetAdditionsExecuted());
    }

    BootstrapableCiphertext Calculus::TanhLeastSquares(const BootstrapableCiphertext &x) const {
        const int decLevels = x.GetRemainingLevels() - 4;
        const auto b = this->EvalBootstrap(BootstrapableCiphertext(x.GetCiphertext(), static_cast<int8_t>(decLevels),
                                                                   x.GetAdditionsExecuted()));

        const std::vector<double> coefficients = {
            0.0,
            0.590585129666859,
            0.0,
            -0.028944353572596,
            0.0,
            0.000501961286007
        };

        auto c = this->GetCc()->EvalPolyLinear(b.GetCiphertext(), coefficients);
        c = this->SafeRescaling(c);
        return BootstrapableCiphertext(c, b.GetRemainingLevels(), b.GetAdditionsExecuted());
    }

    BootstrapableCiphertext Calculus::Sigmoid(const BootstrapableCiphertext &x,
                                              const ApproximationFn approximation) const {
        switch (approximation) {
            case CHEBYSHEV: return this->SigmoidChebyshev(x);
            case TAYLOR: return this->SigmoidTaylor(x);
            case LEAST_SQUARES: return this->SigmoidLeastSquares(x);
            default: return this->SigmoidChebyshev(x);
        }
    }

    BootstrapableCiphertext Calculus::SigmoidDerivative(const BootstrapableCiphertext &x,
                                                        const ApproximationFn approximation) const {
        BootstrapableCiphertext s;

        switch (approximation) {
            case CHEBYSHEV:
                s = this->SigmoidChebyshev(x);
                break;
            case TAYLOR:
                s = this->SigmoidTaylor(x);
                break;
            case LEAST_SQUARES:
                s = this->SigmoidLeastSquares(x);
                break;
        }

        const auto term1 = this->EvalSub(this->constants.One(), s);
        const auto d = this->EvalMult(term1, s);
        return d;
    }

    BootstrapableCiphertext Calculus::Tanh(const BootstrapableCiphertext &x,
                                           const ApproximationFn approximation) const {
        switch (approximation) {
            case CHEBYSHEV: return this->TanhChebyshev(x);
            case TAYLOR: return this->TanhTaylor(x);
            case LEAST_SQUARES: return this->TanhLeastSquares(x);
            default: return this->TanhChebyshev(x);
        }
    }

    BootstrapableCiphertext Calculus::TanhDerivative(const BootstrapableCiphertext &x,
                                                     const ApproximationFn approximation) const {
        BootstrapableCiphertext s;

        switch (approximation) {
            case CHEBYSHEV:
                s = this->TanhChebyshev(x);
                break;
            case TAYLOR:
                s = this->TanhTaylor(x);
                break;
            case LEAST_SQUARES:
                s = this->TanhLeastSquares(x);
                break;
        }

        const auto tanh_squared = this->EvalMult(s, s);
        const auto d = this->EvalSub(this->constants.One(), tanh_squared);
        return d;
    }

    std::vector<std::vector<double> > Calculus::Transpose(const std::vector<std::vector<double> > &mat) {
        const size_t rows = mat.size();
        const size_t cols = mat[0].size();
        std::vector transposed(cols, std::vector<double>(rows));

        for (size_t i = 0; i < rows; ++i) {
            for (size_t j = 0; j < cols; ++j) {
                transposed[j][i] = mat[i][j];
            }
        }

        return transposed;
    }
}
