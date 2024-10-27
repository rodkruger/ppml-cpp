//
// Created by rkruger on 23/10/24.
//

#ifndef MODEL_H
#define MODEL_H

#include "openfhe.h"
#include "core.h"
#include "hemath.h"

namespace hermesml {

    class BgvKnnEncrypted : EncryptedObject {
    private:
        HEContext ctx;
        CalculusQuant calculus;
        CryptoContext<DCRTPoly> cc;
        int32_t k;
        std::vector<Ciphertext<DCRTPoly>> trainingData;
        std::vector<int32_t> trainingLabels;

        Ciphertext<DCRTPoly> Distance(const Ciphertext<DCRTPoly>& point1,
                                      const Ciphertext<DCRTPoly>& point2);

    public:

        BgvKnnEncrypted(int32_t k, HEContext &ctx);

        void Init();

        void Fit(const std::vector<Ciphertext<DCRTPoly>>& trainingData,
                 const std::vector<int32_t>& trainingLabels);

        int32_t Predict(const Ciphertext<DCRTPoly>& dataPoint);

    };

    class CkksKnnEncrypted : EncryptedObject {
    private:
        HEContext ctx;
        Calculus calculus;
        CryptoContext<DCRTPoly> cc;
        int32_t k;
        std::vector<Ciphertext<DCRTPoly>> trainingData;
        std::vector<int32_t> trainingLabels;

        Ciphertext<DCRTPoly> Euclidean(const Ciphertext<DCRTPoly>& point1,
                                       const Ciphertext<DCRTPoly>& point2);

        Ciphertext<DCRTPoly> Distance(const Ciphertext<DCRTPoly>& point1,
                                      const Ciphertext<DCRTPoly>& point2);

    public:

        CkksKnnEncrypted(int32_t k, HEContext &ctx);

        void Init();

        void Fit(const std::vector<Ciphertext<DCRTPoly>>& trainingData,
                 const std::vector<int32_t>& trainingLabels);

        int32_t Predict(const Ciphertext<DCRTPoly>& dataPoint);

    };

    /*
    HEConfig::HEConfig() : HEConfig(2622173311, 20, 30, 8192, 60) {}
    HEConfig::HEConfig(int64_t plaintextModulus, int64_t multiplicativeDepth, int64_t scaleModSize, int64_t batchSize, int64_t firstModSize) {}
    */

}

#endif //MODEL_H
