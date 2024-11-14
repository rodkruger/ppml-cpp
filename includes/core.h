//
// Created by rkruger on 23/10/24.
//

#ifndef CORE_H
#define CORE_H

#define QUANTIZE_SCALE_FACTOR 1e4 // 10^4

#include "context.h"

namespace hermesml {

    class EncryptedObject {
    private:
        HEContext ctx;
        CryptoContext<DCRTPoly> cc;

    public:
        explicit EncryptedObject(const HEContext &ctx);
        HEContext GetCtx() const;
        CryptoContext<DCRTPoly> GetCc() const;
        Ciphertext<DCRTPoly> Encrypt(const std::vector<int64_t>& plaintext) const;
        std::vector<int64_t> UnpackValues(const Plaintext& plaintext, int32_t n_features) const;
        void Snoop(const Ciphertext<DCRTPoly>& ciphertext, int32_t n_features) const;
    };

    class MinMaxScaler {
    public:
        void Scale(std::vector<std::vector<double>>& data);
    };

    class Quantizer {
    public:
        std::vector<std::vector<int64_t>> Quantize(std::vector<std::vector<double>>& data);
        std::vector<int64_t> Quantize(std::vector<double>& data);
    };

}

#endif //CORE_H
