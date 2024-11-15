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
        u_int8_t arithmeticCost;

    public:
        explicit EncryptedObject(HEContext ctx);
        HEContext GetCtx();
        CryptoContext<DCRTPoly> GetCc();
        Ciphertext<DCRTPoly> Encrypt(std::vector<int64_t> plaintext);
        Ciphertext<DCRTPoly> EncryptCKKS(std::vector<double> plaintext);
        std::vector<double> UnpackValues(Plaintext plaintext, int32_t n_features);
        Ciphertext<DCRTPoly> EvalAdd(Ciphertext<DCRTPoly> ciphertext1,
                                     Ciphertext<DCRTPoly> ciphertext2);
        Ciphertext<DCRTPoly> EvalSum(Ciphertext<DCRTPoly> ciphertext1);
        Ciphertext<DCRTPoly> EvalSub(Ciphertext<DCRTPoly> ciphertext1,
                                     Ciphertext<DCRTPoly> ciphertext2);
        Ciphertext<DCRTPoly> EvalMult(Ciphertext<DCRTPoly> ciphertext1, Ciphertext<DCRTPoly> ciphertext2);
        Ciphertext<DCRTPoly> EvalBootstrap(Ciphertext<DCRTPoly> ciphertext);
        void Snoop(Ciphertext<DCRTPoly> ciphertext, int32_t n_features);
        int16_t GetScalingFactor();
    };

    class MinMaxScaler {
    public:
        void Scale(std::vector<std::vector<double>>& data);
    };

    class Quantizer {
    public:
        std::vector<std::vector<int64_t>> Quantize(std::vector<std::vector<double>> data);
        std::vector<int64_t> Quantize(std::vector<double> data);
    };

}

#endif //CORE_H
