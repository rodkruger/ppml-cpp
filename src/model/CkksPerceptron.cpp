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
        const double lr = 0.001 * GetScalingFactor();
        return this->EncryptCKKS(std::vector(n_features, lr));
    }

    BootstrapableCiphertext CkksPerceptron::sigmoid(const BootstrapableCiphertext &x) const {
        //                                                    ( term 1  )   (   term 2   )
        // Approximate sigmoid using cubic polynomial: 0.5  +  0.125 * x  -  0.0625 * x^3
        const auto x_squared = this->EvalMult(x, x);
        const auto x_cubed = this->EvalMult(x_squared, x);

        const auto term1 = this->EvalMult(x, this->constants.C125());
        const auto term2 = this->EvalMult(x_cubed, this->constants.C0625());

        return this->EvalAdd(this->constants.C05(), this->EvalSub(term1, term2));
    }

    void CkksPerceptron::Fit(const std::vector<BootstrapableCiphertext> &x,
                             const std::vector<BootstrapableCiphertext> &y) {
        const auto eLr = this->GetLearningRate();

        // Initialize weights, bias, features, sigmoid and errors to zero
        this->eWeights = this->constants.Zero();
        this->eBias = this->constants.Zero();

        for (int32_t epoch = 0; epoch < this->epochs; epoch++) {
            // Compute encrypted gradients using plain 'y' values
            for (size_t i = 0; i < x.size(); i++) {
                const auto &eFeatureValues = x[i];

                // Compute the linear combination Z for the feature values
                auto eZ = this->EvalMult(eFeatureValues, this->eWeights);
                eZ = this->EvalSum(eZ);
                eZ = this->EvalAdd(eZ, this->eBias);

                // Execute the activation function - in this case, the sigmoid
                const auto eActivation = this->sigmoid(eZ);

                // Compute the error
                const auto eError = this->EvalSub(eActivation, y[i]);

                // Update the weights
                auto eNewWeights = this->EvalMult(eLr, eError);
                eNewWeights = this->EvalMult(eNewWeights, eFeatureValues);
                this->eWeights = this->EvalSub(this->eWeights, eNewWeights);
                // this->Snoop(this->encryptedWeights, this->n_features);
            }
        }
    }

    BootstrapableCiphertext CkksPerceptron::Predict(const BootstrapableCiphertext &x) {
        auto z = this->EvalMult(x, this->eWeights);
        z = this->EvalSum(z);
        z = this->EvalAdd(z, this->eBias);
        return this->sigmoid(z);
    }

    std::vector<BootstrapableCiphertext> CkksPerceptron::PredictAll(const std::vector<BootstrapableCiphertext> &x) {
        std::vector<BootstrapableCiphertext> predictions(x.size());

        for (size_t i = 0; i < x.size(); ++i) {
            predictions[i] = this->Predict(x[i]);
        }

        return predictions;
    }
}
