#include "client.h"

namespace hermesml {
    Client::Client(const HEContext &ctx) : EncryptedObject(ctx) {
    }

    std::vector<Ciphertext<DCRTPoly> > Client::Encrypt(const std::vector<int64_t> &data) const {
        auto encryptedData = std::vector<Ciphertext<DCRTPoly> >();

        for (auto row: data) {
            const auto packedValue = this->GetCc()->MakePackedPlaintext({row});
            const auto encryptedRow = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packedValue);
            encryptedData.push_back(encryptedRow);
        }

        return encryptedData;
    }

    std::vector<Ciphertext<DCRTPoly> > Client::Encrypt(const std::vector<std::vector<int64_t> > &data) const {
        auto encryptedData = std::vector<Ciphertext<DCRTPoly> >();

        for (auto &row: data) {
            const auto packedValue = this->GetCc()->MakePackedPlaintext(row);
            const auto encryptedRow = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packedValue);
            encryptedData.push_back(encryptedRow);
        }

        return encryptedData;
    }

    std::vector<Ciphertext<DCRTPoly> > Client::EncryptCKKS(const std::vector<std::vector<double> > &data) const {
        auto encryptedData = std::vector<Ciphertext<DCRTPoly> >();

        for (auto &row: data) {
            const auto packedValue = this->GetCc()->MakeCKKSPackedPlaintext(row);
            const auto encryptedRow = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packedValue);
            encryptedData.push_back(encryptedRow);
        }

        return encryptedData;
    }

    std::vector<Ciphertext<DCRTPoly> > Client::EncryptCKKS(const std::vector<double> &data) const {
        auto encryptedData = std::vector<Ciphertext<DCRTPoly> >();

        for (auto &row: data) {
            const auto packedValue = this->GetCc()->MakeCKKSPackedPlaintext(std::vector(1, row));
            const auto encryptedRow = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packedValue);
            encryptedData.push_back(encryptedRow);
        }

        return encryptedData;
    }
}
