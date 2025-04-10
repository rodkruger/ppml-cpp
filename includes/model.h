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
        virtual void Fit(const std::vector<BootstrapableCiphertext> &x,
                         const std::vector<BootstrapableCiphertext> &y) = 0;

        virtual BootstrapableCiphertext Predict(const BootstrapableCiphertext &point) = 0;

        virtual ~MlModel() = default;
    };

    class CkksLogisticRegression : public EncryptedObject, public MlModel {
    public:
        enum Activation { TANH, SIGMOID, IDENTITY };

        explicit CkksLogisticRegression(const HEContext &ctx, uint16_t n_features, uint16_t epochs,
                                Activation activation = TANH);

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

        Activation activation;
        uint16_t n_features;
        uint16_t epochs;
        BootstrapableCiphertext eWeights;
        BootstrapableCiphertext eBias;

        void InitWeights();

        [[nodiscard]] BootstrapableCiphertext Identity(const BootstrapableCiphertext &x) const;

        [[nodiscard]] BootstrapableCiphertext Sigmoid(const BootstrapableCiphertext &x) const;

        [[nodiscard]] BootstrapableCiphertext Tanh(const BootstrapableCiphertext &x) const;
    };
}

#endif //MODEL_H
