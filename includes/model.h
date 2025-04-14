//
// Created by rkruger on 23/10/24.
//

#ifndef MODEL_H
#define MODEL_H

#include "openfhe.h"
#include "core.h"
#include "hemath.h"

namespace hermesml {
    enum ActivationFn { TANH, SIGMOID };

    enum ApproximationFn { CHEBYSHEV, TAYLOR, LEAST_SQUARES };

    class MlModel {
    public:
        virtual void Fit(const std::vector<BootstrapableCiphertext> &x,
                         const std::vector<BootstrapableCiphertext> &y) = 0;

        virtual BootstrapableCiphertext Predict(const BootstrapableCiphertext &point) = 0;

        virtual ~MlModel() = default;
    };

    class CkksLogisticRegression : public EncryptedObject, public MlModel {
    public:
        explicit CkksLogisticRegression(const HEContext &ctx, uint16_t n_features, uint16_t epochs,
                                        ActivationFn activation = TANH, ApproximationFn approx = CHEBYSHEV);

        [[nodiscard]] BootstrapableCiphertext GetLearningRate() const;

        void Fit(const std::vector<BootstrapableCiphertext> &x,
                 const std::vector<BootstrapableCiphertext> &y) override;

        void Fit(const std::string &eTrainingFeaturesFilePath, const std::string &eTrainingLabelsFilePath);

        BootstrapableCiphertext Predict(const BootstrapableCiphertext &x) override;

        std::vector<BootstrapableCiphertext> PredictAll(const std::vector<BootstrapableCiphertext> &x);

        std::vector<BootstrapableCiphertext> PredictAll(const std::string &eTestingFeaturesFilePath);

    private:
        Calculus calculus;
        Constants constants;

        ActivationFn activation;
        ApproximationFn approximation;
        uint16_t n_features;
        uint16_t epochs;
        BootstrapableCiphertext eWeights;
        BootstrapableCiphertext eBias;

        void InitWeights();

        [[nodiscard]] BootstrapableCiphertext Activation(const BootstrapableCiphertext &x) const;
    };
}

#endif //MODEL_H
