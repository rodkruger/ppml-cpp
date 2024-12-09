#include "client.h"

namespace hermesml
{
    Client::Client(HEContext ctx) : EncryptedObject(ctx)
    {
    }

    std::vector<Ciphertext<DCRTPoly>> Client::Encrypt(std::vector<int64_t> data)
    {
        auto encryptedData = std::vector<Ciphertext<DCRTPoly>>();

        for (auto row : data)
        {
            auto packedValue = this->GetCc()->MakePackedPlaintext({row});
            auto encryptedRow = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packedValue);
            encryptedData.push_back(encryptedRow);
        }

        return encryptedData;
    }

    std::vector<Ciphertext<DCRTPoly>> Client::Encrypt(std::vector<std::vector<int64_t>> data)
    {
        auto encryptedData = std::vector<Ciphertext<DCRTPoly>>();

        for (auto row : data)
        {
            auto packedValue = this->GetCc()->MakePackedPlaintext(row);
            auto encryptedRow = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packedValue);
            encryptedData.push_back(encryptedRow);
        }

        return encryptedData;
    }

    std::vector<Ciphertext<DCRTPoly>> Client::EncryptCKKS(std::vector<std::vector<double>> data)
    {
        auto encryptedData = std::vector<Ciphertext<DCRTPoly>>();

        for (auto row : data)
        {
            auto packedValue = this->GetCc()->MakeCKKSPackedPlaintext(row);
            auto encryptedRow = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packedValue);
            encryptedData.push_back(encryptedRow);
        }

        return encryptedData;
    }

    std::vector<Ciphertext<DCRTPoly>> Client::EncryptCKKS(std::vector<double> data)
    {
        auto encryptedData = std::vector<Ciphertext<DCRTPoly>>();

        for (auto row : data)
        {
            auto packedValue = this->GetCc()->MakeCKKSPackedPlaintext(std::vector(1, row));
            auto encryptedRow = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), packedValue);
            encryptedData.push_back(encryptedRow);
        }

        return encryptedData;
    }
}
