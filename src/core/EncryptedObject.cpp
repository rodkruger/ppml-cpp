#include "core.h"

namespace hermesml
{
    EncryptedObject::EncryptedObject(const HEContext& ctx)
    {
        this->ctx = ctx;
        this->cc = this->ctx.GetCc();
    }

    CryptoContext<DCRTPoly> EncryptedObject::GetCc() const
    {
        return this->cc;
    }

    HEContext EncryptedObject::GetCtx() const
    {
        return this->ctx;
    }

    Ciphertext<DCRTPoly> EncryptedObject::Encrypt(const std::vector<int64_t>& plaintext) const
    {
        const auto packed = this->GetCc()->MakePackedPlaintext(plaintext);
        return this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packed);
    }

    std::vector<int64_t> EncryptedObject::UnpackValues(const Plaintext& plaintext, const int32_t n_features) const
    {
        auto packed = plaintext->GetPackedValue();
        auto n = n_features <= packed.size() ? n_features : packed.size();

        auto unpacked = std::vector<int64_t>(n);
        std::copy_n(packed.begin(), n, unpacked.begin());

        return unpacked;
    }

    void EncryptedObject::Snoop(const Ciphertext<DCRTPoly>& ciphertext, int32_t n_features) const
    {
        Plaintext plaintext;
        this->GetCc()->Decrypt(this->GetCtx().GetPrivateKey(), ciphertext, &plaintext);
        std::cout << this->UnpackValues(plaintext, n_features) << std::endl;
    }
}
