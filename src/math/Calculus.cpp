#include "hemath.h"

namespace hermesml {

    Calculus::Calculus(const HEContext &ctx) : EncryptedObject(ctx), constants(Constants(ctx)) {}

    Ciphertext<DCRTPoly> Calculus::TaylorSqrt(const Ciphertext<DCRTPoly>& x) {

        Plaintext plaintext;
        Ciphertext<DCRTPoly> ciphertext;
        std::vector<double> inputVec;

        // Taylor Series approximation for sqrt(1 + x)
        // sqrt(1 + x) ≈ 1 + x/2 - x^2/8 + x^3/16 - ...

        inputVec = {0.5};
        plaintext = this->GetCc()->MakeCKKSPackedPlaintext(inputVec);
        ciphertext = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plaintext);

        auto term = this->GetCc()->EvalMult(x,  ciphertext);                // First term: x/2
        auto result = this->GetCc()->EvalAdd(this->constants.One(), term);  // result += 1 + x/2

        if constexpr (TAYLOR_SQRT_PRECISION > 1) {
            inputVec = {(-1.0 / 8.0)};
            plaintext = this->GetCc()->MakeCKKSPackedPlaintext(inputVec);
            ciphertext = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plaintext);

            const auto x2 = this->GetCc()->EvalMult(x, x);  // x^2
            term = this->GetCc()->EvalMult(x2, ciphertext);             // - x^2 / 8
            result = this->GetCc()->EvalAdd(result, term);              // result += -x^2 / 8

            if constexpr (TAYLOR_SQRT_PRECISION > 2) {
                inputVec = {(1.0 / 16.0)};
                plaintext = this->GetCc()->MakeCKKSPackedPlaintext(inputVec);
                ciphertext = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plaintext);

                const auto x3 = this->GetCc()->EvalMult(x2, x);  // x^3
                term = this->GetCc()->EvalMult(x3, ciphertext);             // x^3 / 16
                result = this->GetCc()->EvalAdd(result, term);              // result += x^3 / 16
            }

            // You can continue adding more terms for higher accuracy ...
        }

        return result;
    }

    Ciphertext<DCRTPoly> Calculus::Euclidean(const Ciphertext<DCRTPoly>& point1,
                                             const Ciphertext<DCRTPoly>& point2) {

        auto subb = this->GetCc()->EvalSub(point1, point2);
        auto abss = this->GetCc()->EvalSquare(subb);
        auto summ = this->GetCc()->EvalSum(abss, this->GetCtx().GetNumSlots());
        auto sqrtt = this->TaylorSqrt(summ);
        return summ;
    }
}