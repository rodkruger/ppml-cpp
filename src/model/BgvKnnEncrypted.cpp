#include <context.h>
#include <vector>

#include "openfhe.h"
#include "model.h"

using namespace lbcrypto;

namespace hermesml {

    BgvKnnEncrypted::BgvKnnEncrypted( int32_t k,  HEContext ctx) : EncryptedObject(ctx), calculus(CalculusQuant(ctx)) {
        this->k = k;
        this->ctx = ctx;
    }

    BootstrapableCiphertext BgvKnnEncrypted::Distance( BootstrapableCiphertext point1,
                                                                 BootstrapableCiphertext point2) {
        return this->calculus.Euclidean(point1, point2);
    }

    void BgvKnnEncrypted::Fit(const std::vector<BootstrapableCiphertext>& pTrainingData,
                              const std::vector<BootstrapableCiphertext>& pTrainingLabels) {
        this->trainingData = pTrainingData;
        this->trainingLabels = pTrainingLabels;
    }

    BootstrapableCiphertext BgvKnnEncrypted::Predict(const BootstrapableCiphertext& dataPoint) {
        // Compute distances from the test point to all training points
         size_t numTrainingPoints = trainingData.size();
        std::vector<std::pair<Ciphertext<DCRTPoly>, int64_t>> distances;

        for (size_t i = 0; i < numTrainingPoints; i++) {
            auto distance = this->Distance(this->trainingData[i], dataPoint);
            // TODO: review encrypted labels
            // distances.emplace_back(distance, this->trainingLabels[i]);
        }

        return BootstrapableCiphertext();
    }
}
