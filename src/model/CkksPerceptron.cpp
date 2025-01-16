#include "model.h"

namespace hermesml {
    CkksPerceptron::CkksPerceptron(const HEContext &ctx,
                                   const uint16_t n_features,
                                   const uint16_t epochs): EncryptedObject(ctx), calculus(Calculus(ctx)),
                                                           constants(Constants(ctx, n_features)),
                                                           eWeights(this->constants.Zero()),
                                                           eBias(this->constants.Zero()) {
        this->n_features = n_features;
        this->epochs = epochs;
    }

    BootstrapableCiphertext CkksPerceptron::GetLearningRate() const {
        const double lr = 0.01 * GetScalingFactor();
        return this->EncryptCKKS(std::vector(n_features, lr));
    }

    BootstrapableCiphertext CkksPerceptron::tanh(const BootstrapableCiphertext &x) const {
        // Taylor expansion for hiperbolic tangent - tanh
        // return x - (x**3*0.333333) + (x**5*0.133333)

        const auto x_squared = this->EvalMult(x, x);
        const auto x_cubed = this->EvalMult(x_squared, x);
        const auto x_fived = this->EvalMult(x_squared, x_cubed);

        const auto term1 = this->EvalMult(x_cubed, this->constants.C0333333());
        const auto term2 = this->EvalMult(x_fived, this->constants.C0133333());

        return this->EvalSub(x, this->EvalAdd(term1, term2));
    }

    void CkksPerceptron::Fit(const std::vector<BootstrapableCiphertext> &x,
                             const std::vector<BootstrapableCiphertext> &y) {
        const auto eLr = this->GetLearningRate();

        // Initialize weights and bias
        this->eWeights = this->constants.Zero();
        this->eBias = this->constants.Zero();

        for (int32_t epoch = 0; epoch < this->epochs; epoch++) {
            // Compute encrypted gradients using plain 'y' values
            for (size_t i = 0; i < x.size(); i++) {
                const auto &eFeatureValues = x[i];

                // Execute the activation function
                const auto eActivation = this->Predict(eFeatureValues);

                // Compute the error
                const auto eError = this->EvalSub(eActivation, y[i]);

                // Compute the delta
                auto eDelta = this->EvalMult(eError, eLr);

                // Update the weights
                auto eNewWeights = this->EvalMult(eFeatureValues, eDelta);
                this->eWeights = this->EvalAdd(this->eWeights, eNewWeights);

                // Update the bias
                auto eNewBias = this->EvalAdd(this->eBias, eDelta);
            }
        }
    }

    BootstrapableCiphertext CkksPerceptron::Predict(const BootstrapableCiphertext &x) {
        auto z = this->EvalMult(x, this->eWeights);
        z = this->EvalSum(z);
        z = this->EvalAdd(z, this->eBias);
        return this->tanh(z);
    }

    std::vector<BootstrapableCiphertext> CkksPerceptron::PredictAll(const std::vector<BootstrapableCiphertext> &x) {
        std::vector<BootstrapableCiphertext> predictions(x.size());

        for (size_t i = 0; i < x.size(); ++i) {
            predictions[i] = this->Predict(x[i]);
        }

        return predictions;
    }
}
