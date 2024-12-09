#include "hemath.h"

namespace hermesml
{
    Constants::Constants(HEContext ctx, int32_t n_features) : EncryptedObject(ctx)
    {
        Plaintext plaintext = this->GetCc()->MakeCKKSPackedPlaintext(std::vector<double>(n_features, 0));
        this->encryptedZero = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plaintext);

        plaintext = this->GetCc()->MakeCKKSPackedPlaintext(std::vector<double>(n_features, 1));
        this->encryptedOne = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plaintext);

        plaintext = this->GetCc()->MakeCKKSPackedPlaintext(std::vector<double>(n_features, 0.5));
        this->encrypted05 = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plaintext);

        plaintext = this->GetCc()->MakeCKKSPackedPlaintext(std::vector<double>(n_features, 0.125));
        this->encrypted125 = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plaintext);

        plaintext = this->GetCc()->MakeCKKSPackedPlaintext(std::vector<double>(n_features, 0.0625));
        this->encrypted0625 = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plaintext);
    }

    Ciphertext<DCRTPoly> Constants::Zero()
    {
        return this->encryptedZero;
    }

    Ciphertext<DCRTPoly> Constants::One()
    {
        return this->encryptedOne;
    }

    Ciphertext<DCRTPoly> Constants::C05()
    {
        return this->encrypted05;
    }

    Ciphertext<DCRTPoly> Constants::C125()
    {
        return this->encrypted125;
    }

    Ciphertext<DCRTPoly> Constants::C0625()
    {
        return this->encrypted0625;
    }
}
