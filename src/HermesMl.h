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
        int32_t modulus;
        int32_t multiplicativeDepth;
        CryptoContext<DCRTPoly> cc;
        KeyPair<DCRTPoly> keys;

    public:
        HEConfig();
        HEConfig(int32_t modulus, int32_t multiplicativeDepth);
        std::vector<std::vector<Ciphertext<DCRTPoly>>> encrypt(const std::vector<std::vector<int32_t>>& data);
    };


    class KNeighboursClassifier {
    private:
        int32_t k;
        std::vector<std::vector<Ciphertext<DCRTPoly>>> trainingData;
        std::vector<int32_t> trainingLabels;

        bigintdyn::ubint<unsigned long> manhattan(const std::vector<Ciphertext<DCRTPoly> >& point1,
                                                  const std::vector<Ciphertext<DCRTPoly> >& point2);

    public:
        explicit KNeighboursClassifier(int32_t k);

        bigintdyn::ubint<unsigned long> distance(const std::vector<Ciphertext<DCRTPoly> >& point1,
                                                 const std::vector<Ciphertext<DCRTPoly> >& point2);

        void fit(const std::vector<std::vector<Ciphertext<DCRTPoly>>>& trainingData,
                 const std::vector<int32_t>& trainingLabels);

        int32_t predict(const std::vector<std::vector<Ciphertext<DCRTPoly>>>& testingData);
    };
}

#endif //HERMESML_H
