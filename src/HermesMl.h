//
// Created by rkruger on 15/09/24.
//

#ifndef HERMESML_H
#define HERMESML_H

#include "openfhe.h"

using namespace lbcrypto;

namespace HermesMl {

    class HEConfig {
    public:
        int64_t plaintextModulus;
        int64_t multiplicativeDepth;
        int64_t scaleModSize;
        int64_t batchSize;
        int64_t firstModSize;
        usint numSlots;
        CryptoContext<DCRTPoly> cc;
        KeyPair<DCRTPoly> keys;

        HEConfig();

        HEConfig(int64_t modulus, int64_t multiplicativeDepth, int64_t scaleModSize, int64_t batchSize, int64_t firstModSize);

        CryptoContext<DCRTPoly> getCc() const;

        KeyPair<DCRTPoly> getKeyPair() const;

        std::vector<Ciphertext<DCRTPoly>> encrypt(const std::vector<std::vector<double>>& data);

        void normalize(std::vector<std::vector<double>>& data);
    };


    class KNeighboursClassifier {
    private:
        int64_t k;
        HEConfig heConfig;
        CryptoContext<DCRTPoly> cc;
        PublicKey<DCRTPoly> publicKey;
        Ciphertext<DCRTPoly> cipheredOne;

        std::vector<Ciphertext<DCRTPoly>> trainingData;
        std::vector<int64_t> trainingLabels;

        Ciphertext<DCRTPoly> approximateSqrt(Ciphertext<DCRTPoly> input);

        Ciphertext<DCRTPoly> distance(const Ciphertext<DCRTPoly>& point1,
                                      const Ciphertext<DCRTPoly>& point2);

    public:
        explicit KNeighboursClassifier(int64_t k, const HEConfig& heConfig);

        Ciphertext<DCRTPoly> manhattan(const Ciphertext<DCRTPoly>& point1,
                                       const Ciphertext<DCRTPoly>& point2);

        void fit(const std::vector<Ciphertext<DCRTPoly>>& encryptedTrainingData,
                 const std::vector<int64_t>& trainingLabels);

        int64_t predict(const Ciphertext<DCRTPoly>& encryptedTestingData);
    };
}

#endif //HERMESML_H
