#include "core.h"

namespace hermesml {
    EncryptedObject::EncryptedObject(const HEContext &ctx) {
        this->ctx = ctx;
        this->cc = this->ctx.GetCc();
    }

    int8_t EncryptedObject::ComputeRemainingLevels(const BootstrapableCiphertext &ciphertext1,
                                                   const BootstrapableCiphertext &ciphertext2) {
        auto remainingLevels = ciphertext1.GetRemainingLevels();
        if (ciphertext2.GetRemainingLevels() < remainingLevels) {
            remainingLevels = ciphertext2.GetRemainingLevels();
        }
        return remainingLevels;
    }

    CryptoContext<DCRTPoly> EncryptedObject::GetCc() const {
        return this->cc;
    }

    HEContext EncryptedObject::GetCtx() const {
        return this->ctx;
    }

    BootstrapableCiphertext EncryptedObject::Encrypt(const std::vector<int64_t> &plaintext) const {
        const auto packed = this->GetCc()->MakePackedPlaintext(plaintext);
        auto bCiphertext = BootstrapableCiphertext(this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packed),
                                                   this->GetCtx().GetMultiplicativeDepth());
        bCiphertext.SetRemainingLevels(this->GetCtx().GetMultiplicativeDepth());
        return bCiphertext;
    }

    BootstrapableCiphertext EncryptedObject::EncryptCKKS(const std::vector<double> &plaintext) const {
        const auto packed = this->GetCc()->MakeCKKSPackedPlaintext(plaintext);
        auto bCiphertext = BootstrapableCiphertext(this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packed),
                                                   this->GetCtx().GetMultiplicativeDepth());
        bCiphertext.SetRemainingLevels(this->GetCtx().GetMultiplicativeDepth());
        return bCiphertext;
    }

    std::vector<double> EncryptedObject::UnpackValues(const Plaintext &plaintext, const uint16_t n_features) {
        const auto packed = plaintext->GetCKKSPackedValue();
        const auto n = n_features <= packed.size() ? n_features : packed.size();
        auto unpacked = std::vector<double>();

        for (uint64_t i = 0; i < n; ++i) {
            unpacked.push_back(packed[i].real());
        }

        return unpacked;
    }

    BootstrapableCiphertext EncryptedObject::EvalAdd(const BootstrapableCiphertext &ciphertext1,
                                                     const BootstrapableCiphertext &ciphertext2) const {
        const auto c = this->GetCc()->EvalAdd(ciphertext1.GetCiphertext(), ciphertext2.GetCiphertext());
        const auto additionsExecuted = ciphertext1.GetAdditionsExecuted() + ciphertext2.GetAdditionsExecuted();
        return this->EvalBootstrap(
            BootstrapableCiphertext(c, ComputeRemainingLevels(ciphertext1, ciphertext2), additionsExecuted + 1));
    }

    BootstrapableCiphertext EncryptedObject::EvalSum(const BootstrapableCiphertext &ciphertext1) const {
        const auto c = this->GetCc()->EvalSum(ciphertext1.GetCiphertext(), this->GetCtx().GetNumSlots());
        const auto additionsExecuted = ciphertext1.GetAdditionsExecuted();
        return this->EvalBootstrap(BootstrapableCiphertext(c, ciphertext1.GetRemainingLevels(), additionsExecuted + 1));
    }

    BootstrapableCiphertext EncryptedObject::EvalSub(const BootstrapableCiphertext &ciphertext1,
                                                     const BootstrapableCiphertext &ciphertext2) const {
        const auto c = this->GetCc()->EvalSub(ciphertext1.GetCiphertext(), ciphertext2.GetCiphertext());
        const auto additionsExecuted = ciphertext1.GetAdditionsExecuted() + ciphertext2.GetAdditionsExecuted();
        return this->EvalBootstrap(
            BootstrapableCiphertext(c, ComputeRemainingLevels(ciphertext1, ciphertext2), additionsExecuted + 1));
    }

    BootstrapableCiphertext EncryptedObject::EvalMult(const BootstrapableCiphertext &ciphertext1,
                                                      const BootstrapableCiphertext &ciphertext2) const {
        const auto ciphertext = this->GetCc()->EvalMult(ciphertext1.GetCiphertext(), ciphertext2.GetCiphertext());
        const auto additionsExecuted = ciphertext1.GetAdditionsExecuted() + ciphertext2.GetAdditionsExecuted();

        const int decLevels = ComputeRemainingLevels(ciphertext1, ciphertext2) - 1;

        return this->EvalBootstrap(
            BootstrapableCiphertext(ciphertext, static_cast<int8_t>(decLevels), additionsExecuted));
    }

    BootstrapableCiphertext EncryptedObject::EvalBootstrap(const BootstrapableCiphertext &ciphertext) const {
        if ((ciphertext.GetRemainingLevels() - this->GetCtx().GetEarlyBootstrapping()) <= 1) {
            return BootstrapableCiphertext(this->GetCc()->EvalBootstrap(ciphertext.GetCiphertext()),
                                           this->GetCtx().GetLevelsAfterBootstrapping());
        }

        /*
        if (ciphertext.GetAdditionsExecuted() >= 100) {
            return BootstrapableCiphertext(this->GetCc()->EvalBootstrap(ciphertext.GetCiphertext()),
                                           this->GetCtx().GetLevelsAfterBootstrapping());
        }
        */

        return ciphertext;
    }

    void EncryptedObject::Snoop(const BootstrapableCiphertext &ciphertext, const uint16_t n_features) const {
        Plaintext plaintext;
        this->GetCc()->Decrypt(this->GetCtx().GetPrivateKey(), ciphertext.GetCiphertext(), &plaintext);
        std::cout << UnpackValues(plaintext, n_features) << std::endl;
    }

    int16_t EncryptedObject::GetScalingFactor() {
        return 1;
    }
}
