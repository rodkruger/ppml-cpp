//
// Created by rkruger on 15/09/24.
//

#ifndef HERMESML_H
#define HERMESML_H

#include "openfhe.h"

using namespace lbcrypto;

namespace HermesMl {

    class HEConfig {
    private:
        int64_t modulus;
        int64_t multiplicativeDepth;
        CryptoContext<DCRTPoly> cc;
        KeyPair<DCRTPoly> keys;

    public:
        HEConfig();

        HEConfig(int64_t modulus, int64_t multiplicativeDepth);

        CryptoContext<DCRTPoly> getCc() const;

        KeyPair<DCRTPoly> getKeyPair() const;

        std::vector<std::vector<Ciphertext<DCRTPoly>>> encrypt(const std::vector<std::vector<int64_t>>& data);
    };


    class KNeighboursClassifier {
    private:
        int64_t k;
        HEConfig heConfig;
        std::vector<std::vector<Ciphertext<DCRTPoly>>> trainingData;
        std::vector<int64_t> trainingLabels;

        Ciphertext<DCRTPoly> manhattan(const std::vector<Ciphertext<DCRTPoly> >& point1,
                                       const std::vector<Ciphertext<DCRTPoly> >& point2);

    public:
        explicit KNeighboursClassifier(int64_t k, const HEConfig& heConfig);

        Ciphertext<DCRTPoly> distance(const std::vector<Ciphertext<DCRTPoly> >& point1,
                                      const std::vector<Ciphertext<DCRTPoly> >& point2);

        void fit(const std::vector<std::vector<Ciphertext<DCRTPoly>>>& trainingData,
                 const std::vector<int64_t>& trainingLabels);

        int64_t predict(const std::vector<Ciphertext<DCRTPoly>>& testingData);
    };
}

#endif //HERMESML_H
