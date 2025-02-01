#include "model.h"

namespace hermesml {
    CkksPerceptron::CkksPerceptron(const HEContext &ctx, const uint16_t n_features, const uint16_t epochs,
                                   const Activation activation): EncryptedObject(ctx), calculus(Calculus(ctx)),
                                                                 constants(Constants(ctx, n_features)),
                                                                 activation(activation),
                                                                 n_features(n_features),
                                                                 epochs(epochs),
                                                                 eWeights(this->constants.Zero()),
                                                                 eBias(this->constants.Zero()) {
    }

    BootstrapableCiphertext CkksPerceptron::GetLearningRate() const {
        double lr;

        switch (this->activation) {
            case TANH:
                lr = 0.05;
                break;
            case SIGMOID:
                lr = 0.0025;
                break;
            case IDENTITY:
                lr = 0.001;
                break;
            default:
                lr = 0.01;
        }

        return this->EncryptCKKS(std::vector(this->n_features, lr));
    }

    BootstrapableCiphertext CkksPerceptron::Identity(const BootstrapableCiphertext &x) const {
        // Linear activation function
        return this->EvalAdd(x, this->constants.C05());
    }

    BootstrapableCiphertext CkksPerceptron::Sigmoid(const BootstrapableCiphertext &x) const {
        // Least Squares method for sigmoid approximation
        // return 0.5 + x*0.21689 - x**3*0.0081934 + x**5*0.00016588

        const auto x_squared = this->EvalMult(x, x);
        const auto x_cubed = this->EvalMult(x_squared, x);
        const auto x_fived = this->EvalMult(x_squared, x_cubed);

        const auto term1 = this->EvalMult(x, this->constants.C021689());
        const auto term2 = this->EvalMult(x_cubed, this->constants.C00081934());
        const auto term3 = this->EvalMult(x_fived, this->constants.C000016588());

        const auto result = this->EvalAdd(
            this->EvalSub(
                this->EvalAdd(this->constants.C05(), term1), term2
            ),
            term3
        );

        return result;
    }

    BootstrapableCiphertext CkksPerceptron::Tanh(const BootstrapableCiphertext &x) const {
        // Taylor expansion method for tanh approximation
        // return x - x**3*0.333333 + x**5*0.133333

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
        this->eWeights = eLr;
        this->eBias = eLr;

        for (int32_t epoch = 0; epoch < this->epochs; epoch++) {
            // Compute encrypted gradients using plain 'y' values
            for (size_t i = 0; i < x.size(); i++) {
                const auto &eFeatureValues = x[i];

                // Execute the activation function
                const auto eActivation = this->Predict(eFeatureValues);
                // this->Snoop(eActivation, this->n_features);

                // Compute the error
                const auto eError = this->EvalSub(eActivation, y[i]);

                // Compute the delta
                auto eDelta = this->EvalMult(eError, eLr);

                // Update the weights
                auto eNewWeights = this->EvalMult(eFeatureValues, eDelta);
                this->eWeights = this->EvalAdd(this->eWeights, eNewWeights);

                // Update the bias
                this->eBias = this->EvalAdd(this->eBias, eDelta);
            }
        }
    }

    BootstrapableCiphertext CkksPerceptron::Predict(const BootstrapableCiphertext &x) {
        auto z = this->EvalMult(this->eWeights, x);
        z = this->EvalSum(z);
        z = this->EvalAdd(z, this->eBias);

        switch (this->activation) {
            case SIGMOID:
                return this->Sigmoid(z);
            case TANH:
                return this->Tanh(z);
            case IDENTITY:
                return this->Identity(z);
            default:
                return this->Tanh(z);
        }
    }

    std::vector<BootstrapableCiphertext> CkksPerceptron::PredictAll(const std::vector<BootstrapableCiphertext> &x) {
        std::vector<BootstrapableCiphertext> predictions(x.size());

        for (size_t i = 0; i < x.size(); ++i) {
            predictions[i] = this->Predict(x[i]);
        }

        return predictions;
    }
}
