#include "hemath.h"

namespace hermesml {
    Calculus::Calculus(const HEContext &ctx) : EncryptedObject(ctx), ants(Constants(ctx, 1)) {
    }

    BootstrapableCiphertext Calculus::TaylorSqrt(const BootstrapableCiphertext &x) const {
        // Taylor Series approximation for sqrt(1 + x)
        // sqrt(1 + x) ≈ 1 + x/2 - x^2/8 + x^3/16 - ...

        std::vector inputVec = {0.5};
        auto ciphertext = this->EncryptCKKS(inputVec);

        auto term = this->EvalMult(x, ciphertext); // First term: x/2
        auto result = this->EvalAdd(this->ants.One(), term); // result += 1 + x/2

        if constexpr (TAYLOR_SQRT_PRECISION > 1) {
            inputVec = {(-1.0 / 8.0)};
            ciphertext = this->EncryptCKKS(inputVec);

            const auto x2 = this->EvalMult(x, x); // x^2
            term = this->EvalMult(x2, ciphertext); // - x^2 / 8
            result = this->EvalAdd(result, term); // result += -x^2 / 8

            if constexpr (TAYLOR_SQRT_PRECISION > 2) {
                inputVec = {(1.0 / 16.0)};
                ciphertext = this->EncryptCKKS(inputVec);

                const auto x3 = this->EvalMult(x2, x); // x^3
                term = this->EvalMult(x3, ciphertext); // x^3 / 16
                result = this->EvalAdd(result, term); // result += x^3 / 16
            }

            // You can continue adding more terms for higher accuracy ...
        }

        return result;
    }

    /*
    BootstrapableCiphertext Calculus::Euclidean(const BootstrapableCiphertext &point1,
                                                const BootstrapableCiphertext &point2) const {
        auto subb = this->EvalSub(point1, point2);
        auto abss = this->EvalSquare(subb);
        auto summ = this->EvalSum(abss, this->GetCtx().GetNumSlots());
        auto sqrtt = this->TaylorSqrt(summ);
        return summ;
    }
    */
}
