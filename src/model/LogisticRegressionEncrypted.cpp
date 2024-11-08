#include "model.h"

namespace hermesml
{
    LogisticRegressionEncrypted::LogisticRegressionEncrypted(const HEContext& ctx,
                                                             const int32_t n_features,
                                                             const int32_t epochs):
        EncryptedObject(ctx), calculus(Calculus(ctx)), constants(Constants(ctx, n_features))
    {
        this->n_features = n_features;
        this->epochs = epochs;
    }

    Ciphertext<DCRTPoly> LogisticRegressionEncrypted::sigmoid(const Ciphertext<DCRTPoly>& x) const
    {
        //                                                    ( term 1  )   (   term 2   )
        // Approximate sigmoid using cubic polynomial: 0.5  +  0.125 * x  -  0.0625 * x^3
        const auto x_squared = this->GetCc()->EvalMult(x, x);
        const auto x_cubed = this->GetCc()->EvalMult(x_squared, x);

        const auto term1 = this->GetCc()->EvalMult(x, this->constants.C125());
        const auto term2 = this->GetCc()->EvalMult(x_cubed, this->constants.C0625());

        return this->GetCc()->EvalAdd(this->constants.C05(), this->GetCc()->EvalSub(term1, term2));
    }

    void LogisticRegressionEncrypted::Fit(const std::vector<Ciphertext<DCRTPoly>>& x,
                                          const std::vector<Ciphertext<DCRTPoly>>& y)
    {
        if (!this->GetCc())
        {
            std::cerr << "Error: Cryptographic context `this->GetCc()` is not initialized." << std::endl;
            return;
        }

        constexpr int64_t lr = 0.001 * QUANTIZE_SCALE_FACTOR;
        const auto plainLr = this->GetCc()->MakePackedPlaintext({lr});
        const auto encryptedLr = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plainLr);

        const auto reciprocal = static_cast<int64_t>(std::round(1.0 / x.size() * QUANTIZE_SCALE_FACTOR));
        const auto plainReciprocal = this->GetCc()->MakePackedPlaintext({reciprocal});
        const auto encryptedReciprocal = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plainReciprocal);

        // Initialize weights to zero
        const auto weights = std::vector<int64_t>(this->n_features, 0);
        const auto plainWeights = this->GetCc()->MakePackedPlaintext(weights);
        this->encryptedWeights = this->GetCc()->Encrypt(this->GetCtx().GetPublicKey(), plainWeights);

        for (int32_t epoch = 0; epoch < epochs; epoch++)
        {
            // Perform prediction on all samples at once using SIMD
            std::vector<Ciphertext<DCRTPoly>> y_predictions = PredictAll(x);

            // Initialize the gradient
            auto gradient = this->constants.Zero();

            // Compute encrypted gradients using plain 'y' values
            for (size_t i = 0; i < y_predictions.size(); i++)
            {
                auto encryptedError = this->GetCc()->EvalSub(y_predictions[i], y[i]);
                const auto& packedFeatureValues = x[i];
                auto featureGradient = this->GetCc()->EvalMult(packedFeatureValues, encryptedError);
                auto sumFeatureGradient = this->GetCc()->EvalSum(featureGradient, this->GetCtx().GetNumSlots());
                sumFeatureGradient = this->GetCc()->EvalMult(sumFeatureGradient, encryptedLr);
                sumFeatureGradient = this->GetCc()->EvalMult(sumFeatureGradient, encryptedReciprocal);

                this->encryptedWeights = this->GetCc()->EvalSub(this->encryptedWeights, sumFeatureGradient);
            }
        }
    }

    Ciphertext<DCRTPoly> LogisticRegressionEncrypted::Predict(const Ciphertext<DCRTPoly>& x)
    {
        const auto linear_combination = this->GetCc()->EvalMult(this->encryptedWeights, x);
        return this->sigmoid(linear_combination);
    }

    std::vector<Ciphertext<DCRTPoly>> LogisticRegressionEncrypted::PredictAll(
        const std::vector<Ciphertext<DCRTPoly>>& x)
    {
        std::vector<Ciphertext<DCRTPoly>> predictions(x.size());

        for (size_t i = 0; i < x.size(); ++i)
        {
            predictions[i] = this->Predict(x[i]);
        }

        return predictions;
    }
}
