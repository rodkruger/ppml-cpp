#include "model.h"

void Monitor(const hermesml::BootstrapableCiphertext &packedFeatureValues,
             const hermesml::BootstrapableCiphertext &encryptedWeights,
             const hermesml::BootstrapableCiphertext &z,
             const hermesml::BootstrapableCiphertext &encryptedBias,
             const hermesml::BootstrapableCiphertext &encryptedSigmoid,
             const hermesml::BootstrapableCiphertext &encryptedError,
             const hermesml::BootstrapableCiphertext &encryptedLr,
             const hermesml::BootstrapableCiphertext &newWeights,
             const int32_t point) {
    std::cout << point << " - packedFeatureValues: " << packedFeatureValues.GetCiphertext()->GetLevel() << "; ";
    std::cout << "encryptedWeights: " << encryptedWeights.GetCiphertext()->GetLevel() << "; ";
    std::cout << "z: " << z.GetCiphertext()->GetLevel() << "; ";
    std::cout << "encryptedBias: " << encryptedBias.GetCiphertext()->GetLevel() << "; ";
    std::cout << "encryptedSigmoid: " << encryptedSigmoid.GetCiphertext()->GetLevel() << "; ";
    std::cout << "encryptedError: " << encryptedError.GetCiphertext()->GetLevel() << "; ";
    std::cout << "encryptedLr: " << encryptedLr.GetCiphertext()->GetLevel() << "; ";
    std::cout << "newWeights: " << newWeights.GetCiphertext()->GetLevel() << std::endl;
}

namespace hermesml {
    CkksPerceptron::CkksPerceptron(const HEContext &ctx,
                                   const int16_t n_features,
                                   const int16_t epochs): EncryptedObject(ctx), calculus(Calculus(ctx)),
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
        /*
        Monitor(packedFeatureValues, this->eWeights, z, this->eBias, encryptedSigmoid, encryptedError,
                encryptedLr, newWeights, 1);
        */

        const auto encryptedLr = this->GetLearningRate();

        // Initialize weights to zero
        const auto weights = std::vector<double>(this->n_features, 0);
        this->eWeights = this->EncryptCKKS(weights);

        const auto bias = std::vector<double>(this->n_features, 0);
        this->eBias = this->EncryptCKKS(bias);

        auto packedFeatureValues = this->constants.Zero();
        auto z = this->constants.Zero();
        const auto encryptedSigmoid = this->constants.Zero();
        auto encryptedError = this->constants.Zero();
        auto newWeights = this->constants.Zero();

        for (int32_t epoch = 0; epoch < epochs; epoch++) {
            // Compute encrypted gradients using plain 'y' values
            for (size_t i = 0; i < x.size(); i++) {
                // Compute linear combination
                packedFeatureValues = x[i];
                z = this->EvalMult(packedFeatureValues, this->eWeights);
                z = this->EvalSum(z);
                z = this->EvalAdd(z, this->eBias);

                // Compute sigmoid
                // encryptedSigmoid = this->sigmoid(z);

                // Compute error
                encryptedError = this->EvalSub(encryptedSigmoid, y[i]);

                // Update weights - Wrong calculation using BGV schema
                newWeights = this->EvalMult(encryptedLr, encryptedError);
                newWeights = this->EvalMult(newWeights, packedFeatureValues);
                this->eWeights = this->EvalSub(this->eWeights, newWeights);
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
