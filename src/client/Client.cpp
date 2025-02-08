#include "client.h"

namespace hermesml {
    Client::Client(const HEContext &ctx) : EncryptedObject(ctx) {
    }

    std::vector<BootstrapableCiphertext> Client::Encrypt(const std::vector<int64_t> &data) const {
        auto eData = std::vector<BootstrapableCiphertext>();

        for (auto row: data) {
            const auto pValue = this->GetCc()->MakePackedPlaintext({row});
            const auto eRow = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), pValue);
            const auto bCiphertext = BootstrapableCiphertext(eRow, this->GetCtx().GetMultiplicativeDepth());
            eData.emplace_back(bCiphertext);
        }

        return eData;
    }

    std::vector<BootstrapableCiphertext> Client::Encrypt(const std::vector<std::vector<int64_t> > &data) const {
        auto eData = std::vector<BootstrapableCiphertext>();

        for (auto &row: data) {
            const auto pValue = this->GetCc()->MakePackedPlaintext(row);
            const auto eRow = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), pValue);
            const auto bCiphertext = BootstrapableCiphertext(eRow, this->GetCtx().GetMultiplicativeDepth());
            eData.emplace_back(bCiphertext);
        }

        return eData;
    }

    std::vector<BootstrapableCiphertext> Client::EncryptCKKS(const std::vector<std::vector<double> > &data) const {
        auto eData = std::vector<BootstrapableCiphertext>();

        for (auto &row: data) {
            const auto pValue = this->GetCc()->MakeCKKSPackedPlaintext(row);
            const auto eRow = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), pValue);
            const auto bCiphertext = BootstrapableCiphertext(eRow, this->GetCtx().GetMultiplicativeDepth());
            eData.emplace_back(bCiphertext);
        }

        return eData;
    }

    std::vector<BootstrapableCiphertext> Client::EncryptCKKS(const std::vector<double> &data,
                                                             const size_t n_features) const {
        auto eData = std::vector<BootstrapableCiphertext>();

        for (auto &row: data) {
            const auto pValue = this->GetCc()->MakeCKKSPackedPlaintext(std::vector(n_features, row));
            const auto eRow = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), pValue);
            const auto bCiphertext = BootstrapableCiphertext(eRow, this->GetCtx().GetMultiplicativeDepth());
            eData.emplace_back(bCiphertext);
        }

        return eData;
    }
}
