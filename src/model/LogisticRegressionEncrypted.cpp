#include "model.h"

namespace hermesml {

    LogisticRegressionEncrypted::LogisticRegressionEncrypted(const HEContext &ctx):
      EncryptedObject(ctx), calculus(Calculus(ctx)) { }

    Ciphertext<DCRTPoly> LogisticRegressionEncrypted::sigmoid(const Ciphertext<DCRTPoly>& z) {
      return nullptr;
    }

    void LogisticRegressionEncrypted::Fit(const std::vector<Ciphertext<DCRTPoly>>& trainingData,
                                          const std::vector<int32_t>& trainingLabels) {}

    int32_t LogisticRegressionEncrypted::Predict(const Ciphertext<DCRTPoly>& dataPoint) { return -1; }

}