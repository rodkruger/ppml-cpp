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
        explicit MlModel(uint32_t seed);

        virtual ~MlModel() = default;

        virtual void Fit(const std::vector<BootstrapableCiphertext> &x,
                         const std::vector<BootstrapableCiphertext> &y) = 0;

        virtual BootstrapableCiphertext Predict(const BootstrapableCiphertext &point) = 0;

        [[nodiscard]] uint32_t GetSeed() const;

    private:
        uint32_t seed;
    };

    class CkksLogisticRegression : public EncryptedObject, public MlModel {
    public:
        explicit CkksLogisticRegression(const HEContext &ctx, uint16_t n_features, uint16_t epochs, uint32_t seed = 42,
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

    class CkksNeuralNetwork : public EncryptedObject, public MlModel {
    public:
        explicit CkksNeuralNetwork(const HEContext &ctx, uint16_t n_features, uint16_t epochs,
                                   const std::vector<size_t> &sizes, uint32_t seed = 42, ActivationFn activation = TANH,
                                   ApproximationFn approx = CHEBYSHEV);

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
        std::vector<size_t> layerSizes;
        uint16_t n_features;
        uint16_t epochs;
        std::vector<std::vector<BootstrapableCiphertext> > eWeights;
        std::vector<std::vector<BootstrapableCiphertext> > eBias;
        std::vector<BootstrapableCiphertext> ePreActivations;
        std::vector<BootstrapableCiphertext> eActivations;

        void InitWeights();

        [[nodiscard]] BootstrapableCiphertext Activation(const BootstrapableCiphertext &x) const;

        [[nodiscard]] BootstrapableCiphertext ActivationDerivative(const BootstrapableCiphertext &x) const;
    };
}

#endif //MODEL_H
