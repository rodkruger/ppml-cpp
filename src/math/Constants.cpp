
#include "hemath.h"

namespace hermesml
{
    Constants::Constants(const HEContext& ctx, const int32_t n_features) : EncryptedObject(ctx)
    {
        Plaintext plaintext = this->GetCc()->MakePackedPlaintext(std::vector<int64_t>(n_features, 0));
        this->encryptedZero = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plaintext);

        plaintext = this->GetCc()->MakePackedPlaintext(std::vector<int64_t>(n_features, 1));
        this->encryptedOne = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plaintext);

        plaintext = this->GetCc()->MakePackedPlaintext({
            std::vector(n_features, static_cast<int64_t>(std::round(0.5 * QUANTIZE_SCALE_FACTOR)))
        });
        this->encrypted05 = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plaintext);

        plaintext = this->GetCc()->MakePackedPlaintext({
            std::vector(n_features, static_cast<int64_t>(std::round(0.125 * QUANTIZE_SCALE_FACTOR)))
        });
        this->encrypted125 = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plaintext);

        plaintext = this->GetCc()->MakePackedPlaintext({
            std::vector(n_features, static_cast<int64_t>(std::round(0.0625 * QUANTIZE_SCALE_FACTOR)))
        });
        this->encrypted0625 = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plaintext);
    }

    Ciphertext<DCRTPoly> Constants::Zero() const
    {
        return this->encryptedZero;
    }

    Ciphertext<DCRTPoly> Constants::One() const
    {
        return this->encryptedOne;
    }

    Ciphertext<DCRTPoly> Constants::C05() const
    {
        return this->encrypted05;
    }

    Ciphertext<DCRTPoly> Constants::C125() const
    {
        return this->encrypted125;
    }

    Ciphertext<DCRTPoly> Constants::C0625() const
    {
        return this->encrypted0625;
    }
}
