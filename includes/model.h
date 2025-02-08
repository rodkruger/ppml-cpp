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

    class BgvKnnEncrypted : EncryptedObject, MlModel {
        HEContext ctx;
        CalculusQuant calculus;
        CryptoContext<DCRTPoly> cc;
        int32_t k;
        std::vector<BootstrapableCiphertext> trainingData;
        std::vector<BootstrapableCiphertext> trainingLabels;

        BootstrapableCiphertext Distance(const BootstrapableCiphertext &point1,
                                         const BootstrapableCiphertext &point2);

    public:
        BgvKnnEncrypted(int32_t k, const HEContext &ctx);

        void Fit(const std::vector<BootstrapableCiphertext> &trainingData,
                 const std::vector<BootstrapableCiphertext> &trainingLabels) override;

        BootstrapableCiphertext Predict(const BootstrapableCiphertext &dataPoint) override;
    };

    class CkksKnnEncrypted : EncryptedObject, MlModel {
    private:
        HEContext ctx;
        Calculus calculus;
        CryptoContext<DCRTPoly> cc;
        int32_t k;
        std::vector<Ciphertext<DCRTPoly> > trainingData;
        std::vector<Ciphertext<DCRTPoly> > trainingLabels;

        Ciphertext<DCRTPoly> Distance(const Ciphertext<DCRTPoly> &point1,
                                      const Ciphertext<DCRTPoly> &point2);

    public:
        CkksKnnEncrypted(int32_t k, const HEContext &ctx);

        void Fit(const std::vector<BootstrapableCiphertext> &trainingData,
                 const std::vector<BootstrapableCiphertext> &trainingLabels) override;

        BootstrapableCiphertext Predict(const BootstrapableCiphertext &dataPoint) override;
    };

    /**
     * Logistic Regression is a supervised learning algorithm primarily used for binary classification tasks, where
     * the goal is to predict a binary outcome (e.g., yes/no, spam/ham, etc.). Although called "regression,"
     * Logistic Regression is fundamentally a classification technique.
     *
     * Logistic Regression builds on concepts from Linear Regression but modifies them to output probabilities for
     * classification. In Linear Regression, we model the relationship between the input variables (features) and
     * the target variable as a linear function. However, for binary classification, we need probabilities bound
     * between 0 and 1, which linear regression does not inherently provide. Logistic Regression addresses this
     * through the logistic (sigmoid) function, which maps real-valued inputs to a range between 0 and 1.
     *
     * The core of Logistic Regression is the sigmoid function, defined as 1 / (1 + exp(-z)), where z represents the
     * linear combination of input features and weights (z = w0 + w1 * x1 + w2 * x2 + ... + wn * xn), w0 is the bias
     * (intercept), w1-wn are the weights associated with the features x1 - xn. The sigmoid function squashes the
     * output to a value between 0 and 1, which can be interpreted as the probability of belonging to a particular
     * class.
     *
     *  - If sigmoid(z) >  0.5, the prediction leans towards the positive class.
     *  - If sigmoid(z) <= 0.5, it leans towards the negative class.
     *
     * To train the Logistic Regression model, we need to find the optimal weights w that maximize the accuracy of
     * predictions. The cost function (loss function) in Logistic Regression is the Log-Loss or Binary Cross-Entropy
     * Loss, which is given by:
     *
     *  - double J = -(1.0 / m) * sum(yi * log(hi) + (1 - yi) * log(1 - hi))
     *  where:
     *  - m is the number of training examples
     *  - yi is the actual label for example i
     *  - hi is the predicted probability for example i
     *
     * The Log-Loss penalizes wrong predictions more heavily than right predictions, making it a good fit for binary
     * classification. Logistic Regression typically uses Gradient Descent or a variant like Stochastic Gradient
     * Descent (SGD) for optimization, iteratively adjusting weights to minimize the cost function.
     *
     * In gradient descent, we calculate the gradient of the cost function with respect to each weight wj and update it
     * as follows:
     *
     *  double gradient_wj = (1.0 / m) * sum((hi - yi) * xij);
     *
     * This process is repeated until convergence (i.e., when the cost function stops decreasing significantly).
     *
     * To prevent overfitting, regularization can be applied by adding a penalty term to the cost function,
     * discouraging the model from assigning too much weight to any single feature. The most common types of
     * regularization are L2 Regularization (Ridge) and L1 Regularization (Lasso).
     *
     * double J = -(1.0 / m) * sum(yi * log(hi) + (1 - yi) * log(1 - hi)) + (lambda / 2) * sum(wj^2);
     * where:
     * - lambda is the regularization parameter
     *
     * For multiclass classification (more than two classes), Logistic Regression can be extended to Softmax
     * Regression. In this setup, we use the softmax function to produce a probability distribution over multiple
     * classes. Given a vector z of logits for each class, the softmax function is
     *
     * double softmax_j = exp(z_j) / sum(exp(z_k));
     * where:
     * - z_j is the logit for class j
     * - K is the number of classes
     * - sum(exp(z_k)) is the sum of the exponentials of logits for all classes
     *
     * Advantages:
     * - Simple and interpretable.
     * - Efficient for small to medium datasets.
     * - Provides probabilities, useful for ranking predictions.
     * Limitations:
     * - Assumes a linear relationship between input features and the log-odds of the outcome.
     * - Can be less effective with highly imbalanced datasets.
     * - Struggles with complex relationships or high-dimensional data without feature engineering or transformation.
     */
    class LogisticRegressionEncrypted : public EncryptedObject, public MlModel {
        Calculus calculus;
        Constants constants;

        int32_t n_features;
        int32_t epochs;
        Ciphertext<DCRTPoly> encryptedWeights;
        Ciphertext<DCRTPoly> encryptedBias;

        /**
         * sigmoid(x) = 1 / (1 + e^(-x))
         *
         * However, because computing the exponential function e^(-x) is complex and computationally intensive in
         * homomorphic encryption schemes, a polynomial approximation can be used in its place. A commonly used
         * polynomial approximation for the sigmoid function is a cubic polynomial, such as:
         *
         * sigmoid(x) ≈ 0.5 + 0.125 * x - 0.0625 * x^3
         *
         * This approximation is chosen because it can be computed using only addition and multiplication, which are
         * supported by most of the FHE schemes
         *
         * @param x
        *   a linear combination of input features and weights, also known as the logit (logistic function input)
         * @return
         */
        BootstrapableCiphertext sigmoid(const BootstrapableCiphertext &x);

    public:
        explicit LogisticRegressionEncrypted(const HEContext &ctx, int32_t n_features, int32_t epochs);

        BootstrapableCiphertext GetLearningRate();

        void Fit(const std::vector<BootstrapableCiphertext> &x,
                 const std::vector<BootstrapableCiphertext> &y) override;

        BootstrapableCiphertext Predict(const BootstrapableCiphertext &x) override;

        std::vector<BootstrapableCiphertext> PredictAll(const std::vector<BootstrapableCiphertext> &x);
    };

    class CkksPerceptron : public EncryptedObject, public MlModel {
    public:
        enum Activation { TANH, SIGMOID, IDENTITY };

        explicit CkksPerceptron(const HEContext &ctx, uint16_t n_features, uint16_t epochs,
                                Activation activation = TANH);

        [[nodiscard]] BootstrapableCiphertext GetLearningRate() const;

        void Fit(const std::vector<BootstrapableCiphertext> &x,
                 const std::vector<BootstrapableCiphertext> &y) override;

        BootstrapableCiphertext Predict(const BootstrapableCiphertext &x) override;

        std::vector<BootstrapableCiphertext> PredictAll(const std::vector<BootstrapableCiphertext> &x);

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
