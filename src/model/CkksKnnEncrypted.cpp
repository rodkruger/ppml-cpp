#include "model.h"

using namespace lbcrypto;

namespace hermesml {

    CkksKnnEncrypted::CkksKnnEncrypted(int32_t k, HEContext& ctx) : EncryptedObject(ctx), calculus(Calculus(ctx)) {
        this->k = k;
        this->ctx = ctx;
    }

    Ciphertext<DCRTPoly> CkksKnnEncrypted::Distance(const Ciphertext<DCRTPoly>& point1,
                                                    const Ciphertext<DCRTPoly>& point2) {
        return this->calculus.Euclidean(point1, point2);
    }

    void CkksKnnEncrypted::Fit(const std::vector<Ciphertext<DCRTPoly>>& trainingData,
                               const std::vector<int32_t>& trainingLabels) {
        this->trainingData = trainingData;
        this->trainingLabels = trainingLabels;
    }

    int32_t CkksKnnEncrypted::Predict(const Ciphertext<DCRTPoly>& testingPoint) {

        // Compute distances from the test point to all training points
        size_t numTrainingPoints = trainingData.size();
        std::vector<std::pair<Ciphertext<DCRTPoly>, int64_t>> distances;

        for (size_t i = 0; i < numTrainingPoints; i++) {
            auto distance = this->Distance(this->trainingData[i], testingPoint);
            distances.emplace_back(distance, this->trainingLabels[i]);
        }

        return -1;

        /**
        // Sort the distances
        // CAUTION: this sort may need to be implemented homomorphically!
        std::sort(distances.begin(), distances.end());

        // Iterate over the k-nearest neighbors and count their labels
        std::map<int64_t, int64_t> labelCount;

        for (int i = 0; i < k; ++i) {
            int64_t label = distances[i].second;
            labelCount[label]++;
        }

        // Find the label with the highest count (majority voting)
        int64_t predictedLabel = -1;
        int64_t maxCount = 0;

        for (const auto& label : labelCount) {
            if (label.second > maxCount) {
                maxCount = label.second;
                predictedLabel = label.first;
            }
        }

        return predictedLabel;
        **/
    }
}