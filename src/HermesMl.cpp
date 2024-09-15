#include "openfhe.h"
#include "HermesMl.h"

#include <vector>
#include <iostream>
#include <numeric>

using namespace lbcrypto;

namespace HermesMl {

    HEConfig::HEConfig() : modulus(65537), multiplicativeDepth(2) {}

    HEConfig::HEConfig(int32_t modulus, int32_t multiplicativeDepth) {
        this->modulus = modulus;
        this->multiplicativeDepth = multiplicativeDepth;

        CCParams<CryptoContextBFVRNS> parameters;

        // - **Plaintext Modulus (p)**: Defines the range of plaintext values and ensures operations are performed
        // modulo p. A smaller p reduces noise but limits value range.
        parameters.SetPlaintextModulus(this->modulus);

        // - **Multiplicative Depth**: Specifies the maximum number of sequential homomorphic multiplications can be
        // performed before the ciphertext becomes too noisy to decrypt. Higher depths allow more complex
        // computations but require larger ciphertext moduli, increasing computational cost and memory usage.
        parameters.SetMultiplicativeDepth(this->multiplicativeDepth);

        /// - **Security Level**: Defines the desired level of cryptographic security (e.g., HEStd_128_classic).
        // This parameter determines the size of encryption parameters (such as key size and ciphertext modulus)
        // to ensure a given level of resistance to attacks. Higher security levels (e.g., 128, 192, 256 bits)
        // increase the computational cost but provide stronger encryption.
        parameters.SetSecurityLevel(HEStd_128_classic);

        // - ** Max Relinearization Degree**: Specifies the maximum degree for relinearization in homomorphic
        // operations. Relinearization is used to reduce the size and complexity of ciphertexts after multiplication.
        // A higher degree allows for more complex multiplications but increases computational cost. This parameter
        // helps manage the trade-off between ciphertext size and computational efficiency.
        parameters.SetMaxRelinSkDeg(2);

        // - ** Multiplication Technique **: Specifies the algorithm used for performing homomorphic multiplication.
        // Options include BEHZ, HPS, and FLEXIBLE, each with trade-offs in terms of performance and noise growth.
        // - BEHZ: Optimized for noise management and supports relinearization efficiently.
        // - HPS: Focuses on minimizing computation time and memory usage.
        // - FLEXIBLE: Balances noise growth and performance, adapting to various operation types.
        parameters.SetMultiplicationTechnique(BEHZ);

        // - ** Key Switch Technique **: Determines the method used for key switching, a process that transforms
        // ciphertexts encrypted under one key to another key, typically after multiplication. Options include BV,
        // GHS, and HYBRID, each with trade-offs in efficiency and memory usage.
        // - BV: Brakerski-Vaikuntanathan method, reduces computational complexity but has higher noise growth.
        // - GHS: Gentry-Halevi-Smart method, focuses on minimizing noise at the cost of performance.
        // - HYBRID: Combines the advantages of BV and GHS for balanced performance and noise management.
        parameters.SetKeySwitchTechnique(BV);

        // - ** Ring Dimension **: Specifies the dimension of the polynomial ring used in the encryption scheme,
        // typically as a power of 2. This parameter directly affects the security, noise growth, and performance
        // of homomorphic operations. A larger ring dimension provides higher security and allows more complex
        // computations but increases computational cost and memory usage. The choice of ringDim must balance
        // between security and performance.
        parameters.SetRingDim(8192);

        this->cc = GenCryptoContext(parameters);
        this->cc->Enable(PKE);
        this->cc->Enable(KEYSWITCH);
        this->cc->Enable(LEVELEDSHE);

        // Key generation
        this->keys = this->cc->KeyGen();
        this->cc->EvalMultKeyGen(keys.secretKey);
        this->cc->EvalAtIndexKeyGen(keys.secretKey, {1, -1});
    }

    std::vector<std::vector<Ciphertext<DCRTPoly> > > HEConfig::encrypt(const std::vector<std::vector<int32_t>>& data) {
        auto encryptedData = std::vector<std::vector<Ciphertext<DCRTPoly> >>(data.size());

        for (const auto &point : data) {
            auto encryptedPoint = std::vector<Ciphertext<DCRTPoly>>(point.size());

            for (const auto value : point) {
                Plaintext plaintextPoint = this->cc->MakePackedPlaintext({value});
                encryptedPoint.push_back(this->cc->Encrypt(this->keys.publicKey, plaintextPoint));
            }

            encryptedData.push_back(encryptedPoint);
        }

        return encryptedData;
    }

    KNeighboursClassifier::KNeighboursClassifier(int32_t k) : k(k) {}

    bigintdyn::ubint<unsigned long> KNeighboursClassifier::manhattan(const std::vector<Ciphertext<DCRTPoly> >& point1,
                                                                     const std::vector<Ciphertext<DCRTPoly> >& point2) {
        bigintdyn::ubint<unsigned long> distance = 0;

        for (size_t i = 0; i < point1.size(); ++i) {
            // Calculate the difference between the two coordinates
            auto diff = operator-(point1[i], point2[i]);

            // Square the difference to approximate absolute value
            auto abs_diff = operator*(diff, diff);

            // Add to total distance
            distance = operator+(distance, abs_diff);
        }

        return distance;
    }

    bigintdyn::ubint<unsigned long> KNeighboursClassifier::distance(const std::vector<Ciphertext<DCRTPoly> >& point1,
                                                                    const std::vector<Ciphertext<DCRTPoly> >& point2) {
        return this->manhattan(point1, point2);
    }

    void KNeighboursClassifier::fit(const std::vector<std::vector<Ciphertext<DCRTPoly>>>& trainingData,
                                    const std::vector<int>& trainingLabels) {
        this->trainingData = trainingData;
        this->trainingLabels = trainingLabels;
    }

    int32_t KNeighboursClassifier::predict(const std::vector<std::vector<Ciphertext<DCRTPoly>>>& testingData) {

        // Step 1: Compute distances from the test point to all training points
        size_t numTrainingPoints = testingData.size();
        std::vector<bigintdyn::ubint<unsigned long>> distances;

        for (size_t i = 0; i < numTrainingPoints; ++i) {
            auto distance = this->distance(this->trainingData[i], testingData[i]);
            distances.push_back(distance);
        }

        // Step 2: Sort the distances
        std::vector<size_t> indices(numTrainingPoints);
        iota(indices.begin(), indices.end(), 0);
        sort(distances.begin(), distances.end());

        // Step 3: Determine the majority label among the k-nearest neighbors
        std::vector<int> label_count(2, 0); // Assuming binary classification (0 or 1)

        for (size_t i = 0; i < k; ++i) {
            label_count[this->trainingLabels[indices[i]]]++; // Increment the count for the label of the ith nearest neighbor
        }

        int predicted_label = (label_count[0] > label_count[1]) ? 0 : 1; // Majority vote

        return predicted_label;
    }
}

int main() {

    HermesMl::HEConfig config = HermesMl::HEConfig();

    std::vector<std::vector<int32_t>> trainingData = { {1, 2}, {3, 4}, {5, 6} };
    std::vector<std::vector<int32_t>> testingData = { {2, 3} };
    std::vector<int32_t> trainingLabels = { 0, 1, 0 };

    std::vector<std::vector<Ciphertext<DCRTPoly> > > encryptedTrainingData = config.encrypt(trainingData);
    std::vector<std::vector<Ciphertext<DCRTPoly> > > encryptedTestingData = config.encrypt(testingData);

    HermesMl::KNeighboursClassifier model = HermesMl::KNeighboursClassifier(2);
    model.fit(encryptedTrainingData, trainingLabels);
    model.predict(encryptedTestingData);

    std::cout << "Done!" << std::endl;

    return 0;
}