//
// Created by rkruger on 23/10/24.
//

#ifndef CORE_H
#define CORE_H

#define QUANTIZE_SCALE_FACTOR 1e8 // 10^8

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
    };

    class MinMaxScaler {
    public:
        void Scale(std::vector<std::vector<double>>& data);
    };

    class Quantizer {
    public:
        std::vector<std::vector<int64_t>> Quantize(std::vector<std::vector<double>>& data);
    };

}

#endif //CORE_H
