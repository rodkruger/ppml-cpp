#include "model.h"

void Monitor( Ciphertext<DCRTPoly> packedFeatureValues,
                     Ciphertext<DCRTPoly> encryptedWeights,
                     Ciphertext<DCRTPoly> z,
                     Ciphertext<DCRTPoly> encryptedBias,
                     Ciphertext<DCRTPoly> encryptedSigmoid,
                     Ciphertext<DCRTPoly> encryptedError,
                     Ciphertext<DCRTPoly> encryptedLr,
                     Ciphertext<DCRTPoly> newWeights,
                     int32_t point)
{
    /*
    std::cout << point << " - packedFeatureValues: " << packedFeatureValues->GetLevel() << "; ";
    std::cout << "encryptedWeights: " << encryptedWeights->GetLevel() << "; ";
    std::cout << "z: " << z->GetLevel() << "; ";
    std::cout << "encryptedBias: " << encryptedBias->GetLevel() << "; ";
    std::cout << "encryptedSigmoid: " << encryptedSigmoid->GetLevel() << "; ";
    std::cout << "encryptedError: " << encryptedError->GetLevel() << "; ";
    std::cout << "encryptedLr: " << encryptedLr->GetLevel() << "; ";
    std::cout << "newWeights: " << newWeights->GetLevel() << std::endl;
    */
}

namespace hermesml
{
    LogisticRegressionEncrypted::LogisticRegressionEncrypted( HEContext ctx,
                                                              int32_t n_features,
                                                              int32_t epochs):
        EncryptedObject(ctx), calculus(Calculus(ctx)), constants(Constants(ctx, n_features))
    {
        this->n_features = n_features;
        this->epochs = epochs;
    }

    Ciphertext<DCRTPoly> LogisticRegressionEncrypted::GetLearningRate()
    {
         double lr = 0.001 * this->GetScalingFactor();
        return this->EncryptCKKS(std::vector(n_features, lr));
    }

    Ciphertext<DCRTPoly> LogisticRegressionEncrypted::sigmoid(Ciphertext<DCRTPoly> x)
    {
        //                                                    ( term 1  )   (   term 2   )
        // Approximate sigmoid using cubic polynomial: 0.5  +  0.125 * x  -  0.0625 * x^3
         auto x_squared = this->EvalMult(x, x);
         auto x_cubed = this->EvalMult(x_squared, x);

         auto term1 = this->EvalMult(x, this->constants.C125());
         auto term2 = this->EvalMult(x_cubed, this->constants.C0625());

        return this->EvalAdd(this->constants.C05(), this->EvalSub(term1, term2));
    }

    void LogisticRegressionEncrypted::Fit( std::vector<Ciphertext<DCRTPoly>> x,
                                           std::vector<Ciphertext<DCRTPoly>> y)
    {
         auto encryptedLr = this->GetLearningRate();

        // Initialize weights to zero
         auto weights = std::vector<double>(this->n_features, 0);
        this->encryptedWeights = this->EncryptCKKS(weights);

         auto bias = std::vector<double>(this->n_features, 0);
        this->encryptedBias = this->EncryptCKKS(bias);

        Ciphertext<DCRTPoly> packedFeatureValues = this->constants.Zero();
        Ciphertext<DCRTPoly> z = this->constants.Zero();
        Ciphertext<DCRTPoly> encryptedSigmoid = this->constants.Zero();
        Ciphertext<DCRTPoly> encryptedError = this->constants.Zero();
        Ciphertext<DCRTPoly> newWeights = this->constants.Zero();

        for (int32_t epoch = 0; epoch < epochs; epoch++)
        {
            // Compute encrypted gradients using plain 'y' values
            for (size_t i = 0; i < x.size(); i++)
            {
                // Compute linear combination
                packedFeatureValues = x[i];                                                     Monitor(packedFeatureValues, this->encryptedWeights, z, this->encryptedBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 1);
                z = this->EvalMult(packedFeatureValues, this->encryptedWeights);                Monitor(packedFeatureValues, this->encryptedWeights, z, this->encryptedBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 2);
                z = this->EvalSum(z);                                                           Monitor(packedFeatureValues, this->encryptedWeights, z, this->encryptedBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 3);
                z = this->EvalAdd(z, this->encryptedBias);                                      Monitor(packedFeatureValues, this->encryptedWeights, z, this->encryptedBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 4);

                // Compute sigmoid
                encryptedSigmoid = this->sigmoid(z);                                            Monitor(packedFeatureValues, this->encryptedWeights, z, this->encryptedBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 5);

                // Compute error
                encryptedError = this->EvalSub(encryptedSigmoid, y[i]);                         Monitor(packedFeatureValues, this->encryptedWeights, z, this->encryptedBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 6);

                // Update weights - Wrong calculation using BGV schema
                newWeights = this->EvalMult(encryptedLr, encryptedError);                       Monitor(packedFeatureValues, this->encryptedWeights, z, this->encryptedBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 7);
                newWeights = this->EvalMult(newWeights, packedFeatureValues);                   Monitor(packedFeatureValues, this->encryptedWeights, z, this->encryptedBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 8);
                this->encryptedWeights = this->EvalSub(this->encryptedWeights, newWeights);     Monitor(packedFeatureValues, this->encryptedWeights, z, this->encryptedBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 9);
                // this->Snoop(this->encryptedWeights, this->n_features);
            }
            break;
        }
    }

    Ciphertext<DCRTPoly> LogisticRegressionEncrypted::Predict(Ciphertext<DCRTPoly> x)
    {
        auto z = this->EvalMult(x, this->encryptedWeights);
        z = this->EvalSum(z);
        z = this->EvalAdd(z, this->encryptedBias);
        return this->sigmoid(z);
    }

    std::vector<Ciphertext<DCRTPoly>> LogisticRegressionEncrypted::PredictAll(
         std::vector<Ciphertext<DCRTPoly>> x)
    {
        std::vector<Ciphertext<DCRTPoly>> predictions(x.size());

        for (size_t i = 0; i < x.size(); ++i)
        {
            predictions[i] = this->Predict(x[i]);
        }

        return predictions;
    }
}
