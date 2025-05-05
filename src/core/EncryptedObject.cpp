#include "core.h"

namespace hermesml {
    EncryptedObject::EncryptedObject(const HEContext &ctx) {
        this->ctx = ctx;
        this->cc = this->ctx.GetCc();
    }

    int32_t EncryptedObject::ComputeRemainingLevels(const BootstrapableCiphertext &ciphertext1,
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
        const auto bCiphertext = BootstrapableCiphertext(this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packed),
                                                         static_cast<int32_t>(this->GetCtx().GetMultiplicativeDepth()));
        return bCiphertext;
    }

    BootstrapableCiphertext EncryptedObject::EncryptCKKS(const std::vector<double> &plaintext) const {
        const auto packed = this->GetCc()->MakeCKKSPackedPlaintext(plaintext);
        const auto bCiphertext = BootstrapableCiphertext(this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packed),
                                                         static_cast<int32_t>(this->GetCtx().GetMultiplicativeDepth()));
        return bCiphertext;
    }

    std::vector<BootstrapableCiphertext> EncryptedObject::EncryptCKKS(
        const std::vector<std::vector<double> > &plaintext) const {
        std::vector<BootstrapableCiphertext> bCiphertexts;

        for (auto &row: plaintext) {
            const auto packed = this->GetCc()->MakeCKKSPackedPlaintext(row);
            const auto bCiphertext = BootstrapableCiphertext(
                this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packed),
                static_cast<int32_t>(this->GetCtx().GetMultiplicativeDepth()));
            bCiphertexts.emplace_back(bCiphertext);
        }

        return bCiphertexts;
    }

    std::vector<double> EncryptedObject::UnpackValues(const Plaintext &plaintext) const {
        const auto packed = plaintext->GetCKKSPackedValue();
        const auto n = this->GetCtx().GetNumFeatures() <= packed.size()
                           ? this->GetCtx().GetNumFeatures()
                           : packed.size();
        auto unpacked = std::vector<double>();

        for (uint64_t i = 0; i < n; i++) {
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

    Ciphertext<DCRTPoly> EncryptedObject::SafeRescaling(const Ciphertext<DCRTPoly> &ciphertext) const {
        /* deprecated. nothing to do! */
        return ciphertext;
    }

    BootstrapableCiphertext EncryptedObject::EvalMult(const BootstrapableCiphertext &ciphertext1,
                                                      const BootstrapableCiphertext &ciphertext2) const {
        const auto ciphertext = this->GetCc()->EvalMult(ciphertext1.GetCiphertext(),
                                                        ciphertext2.GetCiphertext());
        const auto additionsExecuted = ciphertext1.GetAdditionsExecuted() + ciphertext2.GetAdditionsExecuted();
        const int decLevels = ComputeRemainingLevels(ciphertext1, ciphertext2) - 1;

        return this->EvalBootstrap(
            BootstrapableCiphertext(ciphertext, static_cast<int8_t>(decLevels), additionsExecuted));
    }

    BootstrapableCiphertext EncryptedObject::EvalBootstrap(const BootstrapableCiphertext &ciphertext) const {
        if ((ciphertext.GetRemainingLevels() - this->GetCtx().GetEarlyBootstrapping()) <= 1) {
            const auto ciphertext2 = this->GetCc()->EvalBootstrap(ciphertext.GetCiphertext());
            return BootstrapableCiphertext(this->SafeRescaling(ciphertext2),
                                           static_cast<int32_t>(this->GetCtx().GetLevelsAfterBootstrapping()));
        }

        return ciphertext;
    }

    void EncryptedObject::Snoop(const BootstrapableCiphertext &ciphertext) const {
        Plaintext plaintext;
        this->GetCc()->Decrypt(this->GetCtx().GetPrivateKey(), ciphertext.GetCiphertext(), &plaintext);
        std::cout << UnpackValues(plaintext) << std::endl;
    }

    int16_t EncryptedObject::GetScalingFactor() {
        return 1;
    }

    BootstrapableCiphertext EncryptedObject::WeightedSum(
        const BootstrapableCiphertext &weights,
        const BootstrapableCiphertext &features,
        const BootstrapableCiphertext &bias) const {
        /* Use only for debugging
        std::cout << "Features:" << std::flush;
        this->Snoop(features, 30);
        std::cout << "Weights:" << std::flush;
        this->Snoop(weights, 30);
        std::cout << "Bias:" << std::flush;
        this->Snoop(bias, 30);
        /* */

        const auto linearDot = this->EvalMult(features, weights);
        const auto sumLinearDot = this->EvalSum(linearDot);
        const auto z = this->EvalAdd(sumLinearDot, bias);

        return z;
    }

    BootstrapableCiphertext EncryptedObject::EvalMerge(
        const std::vector<BootstrapableCiphertext> &ciphertexts) const {
        std::vector<Ciphertext<DCRTPoly> > ciphertextsToMerge;
        auto minRemainingLevel = static_cast<int32_t>(this->GetCtx().GetMultiplicativeDepth());
        for (const auto &c: ciphertexts) {
            ciphertextsToMerge.emplace_back(c.GetCiphertext());
            if (c.GetRemainingLevels() < minRemainingLevel) {
                minRemainingLevel = c.GetRemainingLevels();
            }
        }

        const auto mergedCiphertexts = this->GetCc()->EvalMerge(ciphertextsToMerge);
        const auto b = BootstrapableCiphertext(mergedCiphertexts, minRemainingLevel - 1);

        return b;
    }

    BootstrapableCiphertext EncryptedObject::EvalFlatten(const BootstrapableCiphertext &ciphertext) const {
        std::vector<BootstrapableCiphertext> valuesToReplicate;

        for (auto i = 0; i < this->GetCtx().GetNumSlots(); i++) {
            valuesToReplicate.emplace_back(ciphertext);
        }

        return this->EvalMerge(valuesToReplicate);
    }

    BootstrapableCiphertext EncryptedObject::EvalRotate(const BootstrapableCiphertext &ciphertext,
                                                        const int32_t index) const {
        return BootstrapableCiphertext(this->GetCc()->EvalRotate(ciphertext.GetCiphertext(), index),
                                       ciphertext.GetRemainingLevels());
    }
}
