//
// Created by rkruger on 23/10/24.
//

#ifndef MODEL_H
#define MODEL_H

#include "openfhe.h"
#include "core.h"
#include "hemath.h"

namespace hermesml {

    class MlModel {
    public:
        virtual void Fit(const std::vector<Ciphertext<DCRTPoly>>& trainingData,
                 const std::vector<int32_t>& trainingLabels);

        virtual int32_t Predict(const Ciphertext<DCRTPoly>& dataPoint);

        virtual ~MlModel();
     };

    class BgvKnnEncrypted : EncryptedObject, MlModel {
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
        BgvKnnEncrypted(int32_t k, const HEContext &ctx);

        void Fit(const std::vector<Ciphertext<DCRTPoly>>& trainingData,
                 const std::vector<int32_t>& trainingLabels) override;

        int32_t Predict(const Ciphertext<DCRTPoly>& dataPoint) override;
    };

    class CkksKnnEncrypted : EncryptedObject, MlModel {
    private:
        HEContext ctx;
        Calculus calculus;
        CryptoContext<DCRTPoly> cc;
        int32_t k;
        std::vector<Ciphertext<DCRTPoly>> trainingData;
        std::vector<int32_t> trainingLabels;

        Ciphertext<DCRTPoly> Distance(const Ciphertext<DCRTPoly>& point1,
                                      const Ciphertext<DCRTPoly>& point2);

    public:
        CkksKnnEncrypted(int32_t k, const HEContext &ctx);

        void Fit(const std::vector<Ciphertext<DCRTPoly>>& trainingData,
                 const std::vector<int32_t>& trainingLabels) override;

        int32_t Predict(const Ciphertext<DCRTPoly>& dataPoint) override;
    };

    class LogisticRegressionEncrypted : EncryptedObject, MlModel {
    private:
        HEContext ctx;
        Calculus calculus;
        CryptoContext<DCRTPoly> cc;
        std::vector<Ciphertext<DCRTPoly>> trainingData;
        std::vector<int32_t> trainingLabels;

        Ciphertext<DCRTPoly> sigmoid(const Ciphertext<DCRTPoly>& z);

    public:
        explicit LogisticRegressionEncrypted(const HEContext &ctx);

        void Fit(const std::vector<Ciphertext<DCRTPoly>>& trainingData,
                 const std::vector<int32_t>& trainingLabels) override;

        int32_t Predict(const Ciphertext<DCRTPoly>& dataPoint) override;
    };

}

#endif //MODEL_H
