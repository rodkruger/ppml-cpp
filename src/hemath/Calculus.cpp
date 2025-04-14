#include "hemath.h"

namespace hermesml {
    Calculus::Calculus(const HEContext &ctx) : EncryptedObject(ctx), constants(Constants(ctx, 1)) {
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
        const int decLevels = x.GetRemainingLevels() - 4;

        const auto b = this->
                EvalBootstrap(BootstrapableCiphertext(x.GetCiphertext(), static_cast<int8_t>(decLevels),
                                                      x.GetAdditionsExecuted()));

        auto c = this->GetCc()->EvalChebyshevFunction(
            [](const double x1) { return tanh(x1); }, b.GetCiphertext(),
            -6, 6,
            5);
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
}
