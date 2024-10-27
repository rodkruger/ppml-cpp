#include <context.h>
#include <vector>

#include "openfhe.h"
#include "model.h"

using namespace lbcrypto;

namespace hermesml {

    BgvKnnEncrypted::BgvKnnEncrypted(const int32_t k, const HEContext& ctx) : EncryptedObject(ctx), calculus(CalculusQuant(ctx)) {
        this->k = k;
        this->ctx = ctx;
    }

    Ciphertext<DCRTPoly> BgvKnnEncrypted::Distance(const Ciphertext<DCRTPoly>& point1,
                                                   const Ciphertext<DCRTPoly>& point2) {
        return this->calculus.Euclidean(point1, point2);
    }

    void BgvKnnEncrypted::Fit(const std::vector<Ciphertext<DCRTPoly>>& trainingData,
                              const std::vector<int32_t>& trainingLabels) {
        this->trainingData = trainingData;
        this->trainingLabels = trainingLabels;
    }

    int32_t BgvKnnEncrypted::Predict(const Ciphertext<DCRTPoly>& dataPoint) {
        // Compute distances from the test point to all training points
        const size_t numTrainingPoints = trainingData.size();
        std::vector<std::pair<Ciphertext<DCRTPoly>, int64_t>> distances;

        for (size_t i = 0; i < numTrainingPoints; i++) {
            auto distance = this->Distance(this->trainingData[i], dataPoint);
            distances.emplace_back(distance, this->trainingLabels[i]);
        }

        return -1;
    }

}