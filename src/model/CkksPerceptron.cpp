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
                lr = 0.005;
                break;
            case SIGMOID:
                lr = 0.005;
                break;
            case IDENTITY:
                lr = 0.005;
                break;
            default:
                lr = 0.005;
        }

        return this->EncryptCKKS(std::vector(this->n_features, lr));
    }

    BootstrapableCiphertext CkksPerceptron::Identity(const BootstrapableCiphertext &x) const {
        // Linear activation function
        return this->EvalAdd(x, this->constants.C05());
    }

    BootstrapableCiphertext CkksPerceptron::Sigmoid(const BootstrapableCiphertext &x) const {
        /* Least Squares method for sigmoid approximation */
        /* return 0.5 + x*0.21689 - x**3*0.0081934 + x**5*0.00016588 */

        /*
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
        */

        /* Sigmoid by Chebyshev Approximation */
        constexpr auto levels = 4;
        const auto decLevels = x.GetRemainingLevels() - levels;

        const auto b = this->
                EvalBootstrap(BootstrapableCiphertext(x.GetCiphertext(), static_cast<int8_t>(decLevels),
                                                      x.GetAdditionsExecuted()));

        const auto c = this->GetCc()->EvalLogistic(b.GetCiphertext(), -1.0, 1.0, 5);

        return BootstrapableCiphertext(c, b.GetRemainingLevels(), b.GetAdditionsExecuted());
    }

    BootstrapableCiphertext CkksPerceptron::Tanh(const BootstrapableCiphertext &x) const {
        /* Taylor expansion method for tanh approximation */
        /* return x - x**3*0.333333 + x**5*0.133333 */

        /*
        const auto x_squared = this->EvalMult(x, x);
        const auto x_cubed = this->EvalMult(x_squared, x);
        const auto x_fived = this->EvalMult(x_squared, x_cubed);
        const auto term1 = this->EvalMult(x_cubed, this->constants.C0333333());
        const auto term2 = this->EvalMult(x_fived, this->constants.C0133333());
        return this->EvalSub(x, this->EvalAdd(term1, term2));
        */

        /* Tanh by Chebyshev Approximation */
        const int decLevels = x.GetRemainingLevels() - 4;

        const auto b = this->
                EvalBootstrap(BootstrapableCiphertext(x.GetCiphertext(), static_cast<int8_t>(decLevels),
                                                      x.GetAdditionsExecuted()));

        const auto c = this->GetCc()->EvalChebyshevFunction(
            [](const double x1) { return tanh(x1); }, b.GetCiphertext(),
            -1, 1,
            5);

        return BootstrapableCiphertext(c, b.GetRemainingLevels(), b.GetAdditionsExecuted());
    }

    void CkksPerceptron::InitWeights() {
        /* Use only for debugging purposes
        const auto weights = {
            0.03373467, -0.00025997, -0.03883165, 0.06173363, -0.00649833, 0.04869294, -0.02772681, -0.00845026,
            -0.08583626, -0.05578794, 0.00677842, -0.06297108, 0.016218, -0.0001006, -0.02879055, -0.03708422,
            0.04747317, 0.02290419, 0.02182636, -0.09092148, 0.07830223, -0.01524018, 0.07498792, 0.06051367,
            -0.05838291, -0.01896154, 0.09503015, 0.03494426, -0.04586892, 0.08849982
        };
        /**/

        std::vector<double> weights(this->n_features);
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<double> dist(-0.1, 0.1);

        for (double &w: weights) {
            w = dist(gen);
        }

        this->eWeights = this->EncryptCKKS(weights);
    }

    void CkksPerceptron::Fit(const std::vector<BootstrapableCiphertext> &x,
                             const std::vector<BootstrapableCiphertext> &y) {
        if (x.size() != y.size()) {
            throw std::runtime_error(
                "The size of x must be equal to the size of y. (" + std::to_string(x.size()) + " vs " +
                std::to_string(y.size()) + ")");
        }

        const auto eLr = this->GetLearningRate();

        // Initialize weights and bias
        this->InitWeights();
        this->eBias = this->constants.Zero();

        for (int32_t epoch = 0; epoch < this->epochs; epoch++) {
            // Compute encrypted gradients using plain 'y' values
            for (size_t i = 0; i < x.size(); i++) {
                const auto &eFeatures = x[i];

                // Execute the activation function
                const auto eActivation = this->Predict(eFeatures);

                // Compute the error
                const auto eError = this->EvalSub(y[i], eActivation);

                // Compute the delta
                auto eDelta = this->EvalMult(eError, eLr);

                // Update the weights
                auto eNewWeights = this->EvalMult(eFeatures, eDelta);
                this->eWeights = this->EvalAdd(this->eWeights, eNewWeights);

                // Update the bias
                this->eBias = this->EvalAdd(this->eBias, eDelta);

                /* Use only for debugging purpose
                std::cout << "label: " << std::flush;
                this->Snoop(y[i], this->n_features);

                std::cout << "eActivation: " << std::flush;
                this->Snoop(eActivation, this->n_features);

                std::cout << "Error: " << std::flush;
                this->Snoop(eError, this->n_features);

                std::cout << "eDelta: " << std::flush;
                this->Snoop(eDelta, this->n_features);

                std::cout << "eNewWeights: " << std::flush;
                this->Snoop(eNewWeights, this->n_features);

                std::cout << "this->eWeights: " << std::flush;
                this->Snoop(this->eWeights, this->n_features);

                std::cout << "this->eBias: " << std::flush;
                this->Snoop(this->eBias, this->n_features);

                std::string key;
                std::cout << "Press any key to continue: ";
                std::cin >> key;
                /* */
            }
        }
    }

    void CkksPerceptron::Fit(const std::string &eTrainingFeaturesFilePath,
                             const std::string &eTrainingLabelsFilePath) {
        const auto eLr = this->GetLearningRate();

        // Initialize weights and bias
        this->InitWeights();
        this->eBias = this->constants.Zero();

        for (int32_t epoch = 0; epoch < this->epochs; epoch++) {
            std::ifstream eFeaturesStream(eTrainingFeaturesFilePath, std::ios::binary);
            std::ifstream eLabelsStream(eTrainingLabelsFilePath, std::ios::binary);

            while (eFeaturesStream.peek() != EOF) {
                Ciphertext<DCRTPoly> cipherFeatures;
                Serial::Deserialize(cipherFeatures, eFeaturesStream, SerType::BINARY);
                const auto eFeatures = BootstrapableCiphertext(cipherFeatures, this->GetCtx().GetMultiplicativeDepth());

                Ciphertext<DCRTPoly> cipherLabels;
                Serial::Deserialize(cipherLabels, eLabelsStream, SerType::BINARY);
                const auto eLabels = BootstrapableCiphertext(cipherLabels, this->GetCtx().GetMultiplicativeDepth());

                // Execute the activation function
                const auto eActivation = this->Predict(eFeatures);

                // Compute the error
                const auto eError = this->EvalSub(eLabels, eActivation);

                // Compute the delta
                auto eDelta = this->EvalMult(eError, eLr);

                // Update the weights
                auto eNewWeights = this->EvalMult(eFeatures, eDelta);
                this->eWeights = this->EvalAdd(this->eWeights, eNewWeights);

                // Update the bias
                this->eBias = this->EvalAdd(this->eBias, eDelta);

                /* Use only for debugging purpose
                std::cout << "label: " << std::flush;
                this->Snoop(y[i], this->n_features);

                std::cout << "eActivation: " << std::flush;
                this->Snoop(eActivation, this->n_features);

                std::cout << "Error: " << std::flush;
                this->Snoop(eError, this->n_features);

                std::cout << "eDelta: " << std::flush;
                this->Snoop(eDelta, this->n_features);

                std::cout << "eNewWeights: " << std::flush;
                this->Snoop(eNewWeights, this->n_features);

                std::cout << "this->eWeights: " << std::flush;
                this->Snoop(this->eWeights, this->n_features);

                std::cout << "this->eBias: " << std::flush;
                this->Snoop(this->eBias, this->n_features);

                std::string key;
                std::cout << "Press any key to continue: ";
                std::cin >> key;
                /* */
            }

            eFeaturesStream.close();
            eLabelsStream.close();
        }
    }

    BootstrapableCiphertext CkksPerceptron::Predict(const BootstrapableCiphertext &x) {
        const auto linearDot = this->EvalMult(this->eWeights, x);
        const auto sumLinearDot = this->EvalSum(linearDot);
        const auto sumLinearDotBias = this->EvalAdd(sumLinearDot, this->eBias);

        BootstrapableCiphertext activation;
        switch (this->activation) {
            case SIGMOID:
                activation = this->Sigmoid(sumLinearDotBias);
                break;
            case TANH:
                activation = this->Tanh(sumLinearDotBias);
                break;
            case IDENTITY:
                activation = this->Identity(sumLinearDotBias);
                break;
            default:
                activation = this->Tanh(sumLinearDotBias);
                break;
        }

        /* Use only for debugging purposes
        std::cout << "eFeatures: " << std::flush;
        this->Snoop(x, this->n_features);

        std::cout << "this->eWeights: " << std::flush;
        this->Snoop(this->eWeights, this->n_features);

        std::cout << "linear dot: " << std::flush;
        this->Snoop(linearDot, this->n_features);

        std::cout << "sum: " << std::flush;
        this->Snoop(sumLinearDot, this->n_features);

        std::cout << "sum + bias: " << std::flush;
        this->Snoop(sumLinearDotBias, this->n_features);

        std::cout << "activation: " << std::flush;
        this->Snoop(activation, this->n_features);
        /* */

        return activation;
    }

    std::vector<BootstrapableCiphertext> CkksPerceptron::PredictAll(const std::vector<BootstrapableCiphertext> &x) {
        std::vector<BootstrapableCiphertext> predictions(x.size());

        for (size_t i = 0; i < x.size(); ++i) {
            predictions[i] = this->Predict(x[i]);
        }

        return predictions;
    }

    std::vector<BootstrapableCiphertext> CkksPerceptron::PredictAll(const std::string &eTestingFeaturesFilePath) {
        std::vector<BootstrapableCiphertext> predictions{};

        std::ifstream eFeaturesStream(eTestingFeaturesFilePath, std::ios::binary);

        while (eFeaturesStream.peek() != EOF) {
            Ciphertext<DCRTPoly> eFeatures;
            Serial::Deserialize(eFeatures, eFeaturesStream, SerType::BINARY);
            predictions.emplace_back(
                this->Predict(BootstrapableCiphertext(eFeatures, this->GetCtx().GetMultiplicativeDepth())));
        }

        eFeaturesStream.close();

        return predictions;
    }
}
