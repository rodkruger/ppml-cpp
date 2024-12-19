#include "core.h"

namespace hermesml
{
    EncryptedObject::EncryptedObject(HEContext ctx)
    {
        this->ctx = ctx;
        this->cc = this->ctx.GetCc();
        this->arithmeticCost = 0;
    }

    CryptoContext<DCRTPoly> EncryptedObject::GetCc()
    {
        return this->cc;
    }

    HEContext EncryptedObject::GetCtx()
    {
        return this->ctx;
    }

    Ciphertext<DCRTPoly> EncryptedObject::Encrypt(std::vector<int64_t> plaintext)
    {
        auto packed = this->GetCc()->MakePackedPlaintext(plaintext);
        return this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packed);
    }

    Ciphertext<DCRTPoly> EncryptedObject::EncryptCKKS(std::vector<double> plaintext)
    {
        auto packed = this->GetCc()->MakeCKKSPackedPlaintext(plaintext);
        return this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packed);
    }

    std::vector<double> EncryptedObject::UnpackValues(Plaintext plaintext, uint16_t n_features)
    {
        auto packed = plaintext->GetCKKSPackedValue();
        auto n = n_features <= packed.size() ? n_features : packed.size();
        auto unpacked = std::vector<double>();

        for (uint64_t i = 0; i < n; ++i)
        {
            unpacked.push_back(packed[i].real());
        }

        return unpacked;
    }

    Ciphertext<DCRTPoly> EncryptedObject::EvalAdd(Ciphertext<DCRTPoly> ciphertext1, Ciphertext<DCRTPoly> ciphertext2)
    {
        this->arithmeticCost += 1;
        auto c = this->GetCc()->EvalAdd(ciphertext1, ciphertext2);
        return this->EvalBootstrap(c);
    }

    Ciphertext<DCRTPoly> EncryptedObject::EvalSum(Ciphertext<DCRTPoly> ciphertext1)
    {
        this->arithmeticCost += 2;
        auto c = this->GetCc()->EvalSum(ciphertext1, this->GetCtx().GetNumSlots());
        return this->EvalBootstrap(c);
    }

    Ciphertext<DCRTPoly> EncryptedObject::EvalSub(Ciphertext<DCRTPoly> ciphertext1, Ciphertext<DCRTPoly> ciphertext2)
    {
        this->arithmeticCost += 1;
        auto c = this->GetCc()->EvalSub(ciphertext1, ciphertext2);
        return this->EvalBootstrap(c);
    }

    Ciphertext<DCRTPoly> EncryptedObject::EvalMult(Ciphertext<DCRTPoly> ciphertext1, Ciphertext<DCRTPoly> ciphertext2)
    {
        this->arithmeticCost += 4;
        auto c = this->GetCc()->EvalMult(ciphertext1, ciphertext2);
        return this->EvalBootstrap(c);
    }

    Ciphertext<DCRTPoly> EncryptedObject::EvalBootstrap(Ciphertext<DCRTPoly> ciphertext)
    {
        if (this->arithmeticCost > 16)
        {
            // std::cout << " [ Bootstrapping ] " << std::endl;
            this->arithmeticCost = 0;
            return this->GetCc()->EvalBootstrap(ciphertext);
        }

        return ciphertext;
    }

    void EncryptedObject::Snoop(Ciphertext<DCRTPoly> ciphertext, uint16_t n_features)
    {
        Plaintext plaintext;
        this->GetCc()->Decrypt(this->GetCtx().GetPrivateKey(), ciphertext, &plaintext);
        std::cout << this->UnpackValues(plaintext, n_features) << std::endl;
    }

    int16_t EncryptedObject::GetScalingFactor()
    {
        return 1;
    }
}
