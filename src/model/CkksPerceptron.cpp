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
    CkksPerceptron::CkksPerceptron( HEContext ctx,
                                                              int16_t n_features,
                                                              int16_t epochs):
        EncryptedObject(ctx), calculus(Calculus(ctx)), constants(Constants(ctx, n_features))
    {
        this->n_features = n_features;
        this->epochs = epochs;
    }

    Ciphertext<DCRTPoly> CkksPerceptron::GetLearningRate()
    {
        double lr = 0.001 * this->GetScalingFactor();
        return this->EncryptCKKS(std::vector(n_features, lr));
    }

    /*
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
     */

    void CkksPerceptron::Fit( std::vector<Ciphertext<DCRTPoly>> x,
                                           std::vector<Ciphertext<DCRTPoly>> y)
    {
         auto encryptedLr = this->GetLearningRate();

        // Initialize weights to zero
         auto weights = std::vector<double>(this->n_features, 0);
        this->eWeights = this->EncryptCKKS(weights);

         auto bias = std::vector<double>(this->n_features, 0);
        this->eBias = this->EncryptCKKS(bias);

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
                packedFeatureValues = x[i];                                        Monitor(packedFeatureValues, this->eWeights, z, this->eBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 1);
                z = this->EvalMult(packedFeatureValues, this->eWeights);           Monitor(packedFeatureValues, this->eWeights, z, this->eBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 2);
                z = this->EvalSum(z);                                              Monitor(packedFeatureValues, this->eWeights, z, this->eBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 3);
                z = this->EvalAdd(z, this->eBias);                                 Monitor(packedFeatureValues, this->eWeights, z, this->eBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 4);

                // Compute sigmoid
                encryptedSigmoid = this->sigmoid(z);                               Monitor(packedFeatureValues, this->eWeights, z, this->eBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 5);

                // Compute error
                encryptedError = this->EvalSub(encryptedSigmoid, y[i]);            Monitor(packedFeatureValues, this->eWeights, z, this->eBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 6);

                // Update weights - Wrong calculation using BGV schema
                newWeights = this->EvalMult(encryptedLr, encryptedError);          Monitor(packedFeatureValues, this->eWeights, z, this->eBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 7);
                newWeights = this->EvalMult(newWeights, packedFeatureValues);      Monitor(packedFeatureValues, this->eWeights, z, this->eBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 8);
                this->eWeights = this->EvalSub(this->eWeights, newWeights);        Monitor(packedFeatureValues, this->eWeights, z, this->eBias, encryptedSigmoid, encryptedError, encryptedLr, newWeights, 9);
                // this->Snoop(this->encryptedWeights, this->n_features);
            }
            break;
        }
    }

    Ciphertext<DCRTPoly> CkksPerceptron::Predict(Ciphertext<DCRTPoly> x)
    {
        auto z = this->EvalMult(x, this->eWeights);
        z = this->EvalSum(z);
        z = this->EvalAdd(z, this->eBias);
        return this->sigmoid(z);
    }

    std::vector<Ciphertext<DCRTPoly>> CkksPerceptron::PredictAll(
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
