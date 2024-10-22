#include <chrono>
#include <iostream>
#include <vector>

#include "csv.h"
#include "openfhe.h"
#include "HermesMl.h"

using namespace lbcrypto;

namespace HermesMl {

    //-----------------------------------------------------------------------------------------------------------------

    HEConfig::HEConfig() : HEConfig(2622173311, 20, 30, 8192, 60) {}

    HEConfig::HEConfig(int64_t plaintextModulus, int64_t multiplicativeDepth, int64_t scaleModSize, int64_t batchSize, int64_t firstModSize) {
        // this->plaintextModulus = plaintextModulus;       // Not used in CKKS
        this->multiplicativeDepth = multiplicativeDepth;
        this->scaleModSize = scaleModSize;
        this->batchSize = batchSize;
        // this->firstModSize = firstModSize;               // Not used in CKKS

        CCParams<CryptoContextCKKSRNS> parameters;

        /// - **Security Level**: Defines the desired level of cryptographic security (e.g., HEStd_128_classic).
        // This parameter determines the size of encryption parameters (such as key size and ciphertext modulus)
        // to ensure a given level of resistance to attacks. Higher security levels (e.g., 128, 192, 256 bits)
        // increase the computational cost but provide stronger encryption.
        parameters.SetSecurityLevel(HEStd_128_classic);

        // The scalingModSize parameter determines the bit-length of the scaling modulus in CKKS. It controls the
        // precision of the encrypted floating-point numbers. A higher scalingModSize value improves precision but
        // increases computational cost and ciphertext size. Setting an appropriate scalingModSize is important to
        // balance precision and performance in homomorphic operations.
        parameters.SetScalingModSize(this->scaleModSize);
        //parameters.SetScalingTechnique(FLEXIBLEAUTO);

        // - **Multiplicative Depth**: Specifies the maximum number of sequential homomorphic multiplications can be
        // performed before the ciphertext becomes too noisy to decrypt. Higher depths allow more complex
        // computations but require larger ciphertext moduli, increasing computational cost and memory usage.
        std::vector<uint32_t> levelBudget = {4, 4};
        uint32_t levelsAvailableAfterBootstrap = 10;
        this->multiplicativeDepth = levelsAvailableAfterBootstrap + FHECKKSRNS::GetBootstrapDepth(levelBudget, parameters.GetSecretKeyDist());
        parameters.SetMultiplicativeDepth(this->multiplicativeDepth);

        this->cc = GenCryptoContext(parameters);

        // Enable Public Key Encryption (PKE) functionality in the crypto context. This allows the context to support
        // basic encryption, decryption, and key generation operations.
        this->cc->Enable(PKE);

        // Enable key switching functionality in the crypto context. Key switching allows ciphertexts encrypted under
        // one key to be transformed into ciphertexts encrypted under another key. It's essential for advanced
        // homomorphic operations like relinearization after multiplication or vector rotation.
        this->cc->Enable(KEYSWITCH);

        // Enable Leveled Homomorphic Encryption (LEVELEDSHE) functionality. This enables the crypto context to
        // support a fixed number of homomorphic operations (e.g., additions and multiplications) without requiring
        // bootstrapping. It's useful when you know the maximum depth of computations required in advance, optimizing
        // performance by avoiding the need for bootstrapping.
        this->cc->Enable(LEVELEDSHE);

        // Enable Advanced Somewhat Homomorphic Encryption (ADVANCEDSHE) functionality in the crypto context.
        // This allows for more advanced homomorphic operations, such as rotations and more complex multiplications.
        // These operations typically require additional keys (e.g., rotation keys) and enable more sophisticated
        // computations on encrypted data, like matrix-vector multiplications and convolutions.
        this->cc->Enable(ADVANCEDSHE);

        // Enable Fully Homomorphic Encryption (FHE) functionality in the crypto context. This allows advanced
        // operations such as bootstrapping, which refreshes ciphertexts and reduces accumulated noise, enabling
        // an unlimited number of operations on encrypted data. Bootstrapping is required for deep computations where
        // the noise level in ciphertexts needs to be reset after multiple homomorphic operations.
        this->cc->Enable(FHE);

        // EvalBootstrapSetup: This function sets up the parameters required for bootstrapping in CKKS or similar FHE
        // schemes. Bootstrapping is used to refresh ciphertexts by reducing accumulated noise, enabling further
        // homomorphic operations without decryption. The setup involves configuring key parameters such as the number
        // of levels, scaling factors, and the number of slots (data elements) to be supported during the bootstrap.
        // Proper configuration is crucial to ensure that bootstrapping can handle the desired precision, data size,
        // and the number of operations, while maintaining efficiency. Misconfiguration may lead to ineffective noise
        // reduction or high computational costs during bootstrapping, so it should be carefully tuned based on the
        // application’s needs.
        this->cc->EvalBootstrapSetup(levelBudget);

        // Key generation
        this->keys = this->cc->KeyGen();

        // EvalMultKeyGen generates the evaluation (relinearization) keys required for performing homomorphic
        // ciphertext-ciphertext multiplication. After multiplying two ciphertexts, the resulting ciphertext has a
        // higher degree, which increases computational complexity and noise. The generated evaluation keys allow
        // relinearization, reducing the degree of the ciphertext back to its original form, ensuring efficient and
        // manageable further homomorphic operations.
        this->cc->EvalMultKeyGen(keys.secretKey);

        // Generate the evaluation (summation) keys needed for homomorphic sum operations. These keys allow for the
        // efficient summation of elements in packed ciphertexts, enabling operations such as computing the sum of a
        // vector of encrypted values. The summation keys are necessary for performing homomorphic summation across
        // slots in SIMD (Single Instruction, Multiple Data) packed ciphertexts.
        this->cc->EvalSumKeyGen(keys.secretKey);

        // EvalAtIndexKeyGen generates evaluation keys for rotating the elements of an encrypted vector. The rotation
        // indices specify by how many positions the vector should be shifted (positive for left rotation, negative
        // for right rotation). These rotation keys enable homomorphic operations that require shifting data, such as
        // convolutions or dot products, to be applied directly on the encrypted data.
        // this->cc->EvalAtIndexKeyGen(keys.secretKey, {1, -1});

        // EvalBootstrapKeyGen generates the bootstrapping keys required to perform the bootstrapping process, which
        // refreshes a ciphertext by reducing its noise. This allows for continued homomorphic operations on the
        // encrypted data without the ciphertext becoming too noisy to decrypt correctly. It is essential when
        // performing many operations on encrypted data to maintain accuracy.
        usint ringDim = this->cc->GetRingDimension();
        this->numSlots = ringDim / 2;   // This is the maximum number of slots that can be used for full packing.
        this->cc->EvalBootstrapKeyGen(keys.secretKey, this->numSlots);
    }

    CryptoContext<DCRTPoly> HEConfig::getCc() const { return this->cc; }

    KeyPair<DCRTPoly> HEConfig::getKeyPair() const { return this->keys; }

    std::vector<Ciphertext<DCRTPoly>> HEConfig::encrypt(const std::vector<std::vector<double>>& data) {
        auto encryptedData = std::vector<Ciphertext<DCRTPoly>>();

        for (const auto &row : data) {
            auto packedValue = this->cc->MakeCKKSPackedPlaintext(row);
            auto encryptedRow = this->cc->Encrypt(this->keys.publicKey, packedValue);
            encryptedData.push_back(encryptedRow);
        }

        return encryptedData;
    }

    void HEConfig::normalize(std::vector<std::vector<double>>& data) {
        size_t numRows = data.size();
        size_t numCols = data[0].size();

        for (size_t col = 0; col < numCols; ++col) {
            double minVal = data[0][col];
            double maxVal = data[0][col];

            for (size_t row = 1; row < numRows; ++row) {
                if (data[row][col] < minVal) {
                    minVal = data[row][col];
                }
                if (data[row][col] > maxVal) {
                    maxVal = data[row][col];
                }
            }

            for (size_t row = 0; row < numRows; ++row) {
                if (maxVal - minVal != 0) {
                    data[row][col] = (data[row][col] - minVal) / (maxVal - minVal);
                } else {
                    data[row][col] = 0.0;
                }
            }
        }
    }

    //-----------------------------------------------------------------------------------------------------------------

    KNeighboursClassifier::KNeighboursClassifier(int64_t k, const HEConfig& heConfig) : k(k), heConfig(heConfig) {
        this->cc = heConfig.getCc();
        this->publicKey = heConfig.getKeyPair().publicKey;

        std::vector<double> initialGuess = {1.0};
        Plaintext ptInitialGuess = this->cc->MakeCKKSPackedPlaintext(initialGuess);
        this->cipheredOne = this->cc->Encrypt(this->publicKey, ptInitialGuess);
    }

    Ciphertext<DCRTPoly> KNeighboursClassifier::approximateSqrt(Ciphertext<DCRTPoly> input) {

        uint32_t iterations = 3;

        auto one = this->cipheredOne;
        auto x = this->cc->EvalSub(input, one);  // x = input - 1

        // Step 2: Compute Taylor series approximation for sqrt(1 + x)
        // sqrt(1 + x) ≈ 1 + x/2 - x^2/8 + x^3/16 - ...
        auto result = one;  // Start with 1 (the first term)
        auto term = this->cc->EvalMult(x, 0.5);  // First term: x/2

        result = this->cc->EvalAdd(result, term);  // result += x/2

        if (iterations > 1) {
            // Second term: - x^2 / 8
            auto x2 = cc->EvalMult(x, x);  // x^2
            term = cc->EvalMult(x2, -1.0 / 8.0);  // - x^2 / 8
            result = cc->EvalAdd(result, term);  // result += -x^2 / 8

            if (iterations > 2) {
                // Third term: x^3 / 16
                auto x3 = cc->EvalMult(x2, x);  // x^3
                term = cc->EvalMult(x3, 1.0 / 16.0);  // x^3 / 16
                result = cc->EvalAdd(result, term);  // result += x^3 / 16
            }

            // You can continue adding more terms for higher accuracy...
        }

        return result;  // Return the approximation
    }

    Ciphertext<DCRTPoly> KNeighboursClassifier::manhattan(const Ciphertext<DCRTPoly>& point1,
                                                          const Ciphertext<DCRTPoly>& point2) {

        auto diff = this->cc->EvalSub(point1, point2);
        auto powerr = this->cc->EvalSquare(diff);
        auto summ = this->cc->EvalSum(powerr, this->heConfig.numSlots);
        return this->approximateSqrt(summ);
    }

    Ciphertext<DCRTPoly> KNeighboursClassifier::distance(const Ciphertext<DCRTPoly>& point1,
                                                         const Ciphertext<DCRTPoly>& point2) {

        auto value = this->manhattan(point1, point2);
        return value;
    }

    void KNeighboursClassifier::fit(const std::vector<Ciphertext<DCRTPoly>>& trainingData,
                                    const std::vector<int64_t>& trainingLabels) {
        this->trainingData = trainingData;
        this->trainingLabels = trainingLabels;
    }

    int64_t KNeighboursClassifier::predict(const Ciphertext<DCRTPoly>& testingPoint) {

        // Compute distances from the test point to all training points
        size_t numTrainingPoints = trainingData.size();
        std::vector<std::pair<Ciphertext<DCRTPoly>, int64_t>> distances;

        for (size_t i = 0; i < numTrainingPoints; i++) {
            auto distance = this->distance(this->trainingData[i], testingPoint);
            distances.emplace_back(distance, this->trainingLabels[i]);
        }

        /**
        // Sort the distances
        // CAUTION: this sort may need to be implemented homomorphically!
        std::sort(distances.begin(), distances.end());

        // Iterate over the k-nearest neighbors and count their labels
        std::map<int64_t, int64_t> labelCount;

        for (int i = 0; i < k; ++i) {
            int64_t label = distances[i].second;
            labelCount[label]++;
        }

        // Find the label with the highest count (majority voting)
        int64_t predictedLabel = -1;
        int64_t maxCount = 0;

        for (const auto& label : labelCount) {
            if (label.second > maxCount) {
                maxCount = label.second;
                predictedLabel = label.first;
            }
        }

        return predictedLabel;
        **/
        return -1;
    }
}

//-----------------------------------------------------------------------------------------------------------------

int main() {

    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed;
    HermesMl::HEConfig heConfig = HermesMl::HEConfig();

    // C L I E N T   S I D E   P R O C E S S I N G --------------------------------------------------------------------

    std::cout << ">>> CLIENT SIDE PROCESSING" << std::endl;

    // Step 01
    // Read dataset

    std::cout << "# Read dataset " << std::endl;

    auto wineData = std::vector<std::vector<double>>();
    auto wineLabels = std::vector<int64_t>();

    int classLabel;
    double alcohol, malicAcid, ash, alcalinityOfAsh, magnesium, totalPhenols, flavanoids,
           nonflavonoidPhenols, proanthocyanins, colorIntensity, hue, odOfDilutedWines, proline;

    io::CSVReader<14> in("../datasets/wine/wine.data");

    while (in.read_row(classLabel, alcohol, malicAcid, ash, alcalinityOfAsh, magnesium,
        totalPhenols, flavanoids, nonflavonoidPhenols, proanthocyanins, colorIntensity, hue,
        odOfDilutedWines, proline)) {

        std::vector<double> row = {alcohol, malicAcid, ash, alcalinityOfAsh, magnesium, totalPhenols,
            flavanoids, nonflavonoidPhenols, proanthocyanins, colorIntensity, hue,
            odOfDilutedWines, proline};

        wineData.push_back(row);
        wineLabels.push_back(classLabel);
    }

    heConfig.normalize(wineData);

    //-----------------------------------------------------------------------------------------------------------------

    // Step 02
    // Split dataset in training and testing. Holdout (70% training; 30% testing)

    std::cout << "# Split dataset (70% training; 30% testing)" << std::endl;

    int64_t trainingLength = wineData.size() * 0.7;
    int64_t testingLength = wineData.size() - trainingLength;

    auto trainingData = std::vector<std::vector<double>>();
    auto testingData = std::vector<std::vector<double>>();

    auto trainingLabels = std::vector<int64_t>();
    auto testingLabels = std::vector<int64_t>();

    trainingData.reserve(trainingLength);
    trainingLabels.reserve(testingLength);
    testingData.reserve(testingLength);
    testingLabels.reserve(testingLength);

    std::copy(wineData.begin(), wineData.begin() + trainingLength, std::back_inserter(trainingData));
    std::copy(wineLabels.begin(), wineLabels.begin() + trainingLength, std::back_inserter(trainingLabels));
    std::copy(wineData.begin() + trainingLength, wineData.end(), std::back_inserter(testingData));
    std::copy(wineLabels.begin() + trainingLength, wineLabels.end(), std::back_inserter(testingLabels));

    std::cout << "      Total samples: " << (trainingData.size() + testingData.size()) << std::endl;
    std::cout << "      Training length: " << trainingData.size() << std::endl;
    std::cout << "      Testing length: " << testingData.size() << std::endl;

    //-----------------------------------------------------------------------------------------------------------------

    // Step 03
    // Generating keys and crypto context. Do not share it with anybody :)

    std::cout << "# Generate crypto context" << std::endl;

    auto keyPair = heConfig.getKeyPair();
    auto publicKey = keyPair.publicKey;
    auto privateKey = keyPair.secretKey;
    auto cc = heConfig.getCc();

    std::cout << "      Ring dimension: " << cc->GetRingDimension() << std::endl;
    std::cout << "      Modulus: " << cc->GetModulus() << std::endl;
    std::cout << "      Multiplicative depth: " << heConfig.multiplicativeDepth << std::endl;

    //-----------------------------------------------------------------------------------------------------------------

    // Step 04
    // Encrypt training data

    std::cout << "# Encrypt training data ... " << std::flush;

    start = std::chrono::high_resolution_clock::now();

    auto encryptedTrainingData = heConfig.encrypt(trainingData);

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

    //-----------------------------------------------------------------------------------------------------------------

    // Step 05
    // Encrypt testing data

    std::cout << "# Encrypt testing data ... " << std::flush;

    start = std::chrono::high_resolution_clock::now();

    auto encryptedTestingData = heConfig.encrypt(testingData);

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

    // S E R V E R   S I D E   P R O C E S S I N G --------------------------------------------------------------------

    std::cout << ">>> SERVER SIDE PROCESSING" << std::endl;

    auto clf = HermesMl::KNeighboursClassifier(5, heConfig);
    clf.fit(encryptedTrainingData, trainingLabels);
    clf.predict(encryptedTestingData[0]);
}

/*
std::cout << "Calculations in plaintext" << std::endl;

// Executed in plaintext
std::vector<double> alcoholColumn = std::vector<double>();
std::vector<double> acidMalicColumn = std::vector<double>();
double sum = 0.0;

for (const auto& row : trainingData) {
    auto alcoholValue = row[0];
    auto acidMalicValue = row[1];
    auto value = alcoholValue * acidMalicValue;

    sum += value;

    alcoholColumn.push_back(alcoholValue);
    acidMalicColumn.push_back(acidMalicValue);
}

std::cout << "Result: " << sum << std::endl;
*/

/*
// Executed the sum in ciphered text, using batches
std::cout << std::endl << "Calculations in ciphertext using CKKS" << std::endl;
std::cout << std::endl;

std::cout << "<<< Before multiplication >>>" << std::endl;
std::cout << "Scaling factor: " << cipherAlcoholColumn->GetScalingFactor() << std::endl;
std::cout << "Multiplicative depth: " << cipherAlcoholColumn->GetLevel() << std::endl;
std::cout << "Scaling factor degree: " << cipherAlcoholColumn->GetNoiseScaleDeg() << std::endl;
std::cout << std::endl;

auto ciphertextProduct = cc->EvalMult(cipherAlcoholColumn, cipherAcidMalicColumn);

std::cout << "<<< After multiplication >>>" << std::endl;
std::cout << "Scaling factor: " << ciphertextProduct->GetScalingFactor() << std::endl;
std::cout << "Multiplicative depth: " << ciphertextProduct->GetLevel() << std::endl;
std::cout << "Scaling factor degree: " << ciphertextProduct->GetNoiseScaleDeg() << std::endl;
std::cout << std::endl;

auto ciphertextSum = cc->EvalSum(ciphertextProduct, alcoholColumn.size());

std::cout << "<<< After sum >>>" << std::endl;
std::cout << "Scaling factor: " << ciphertextSum->GetScalingFactor() << std::endl;
std::cout << "Number of scalings performed: " << ciphertextSum->GetLevel() << std::endl;
std::cout << "Scaling factor degree: " << ciphertextSum->GetNoiseScaleDeg() << std::endl;
std::cout << std::endl;

// Step 7: Apply bootstrapping to refresh the ciphertext (reduce noise)
// ciphertextSum = cc->EvalBootstrap(ciphertextSum);

// Step 8: Decrypt the result and display the output
Plaintext result;
cc->Decrypt(keyPair.secretKey, ciphertextSum, &result);
result->SetLength(1);  // Since we expect a single summed value

// Display the result (the sum of all values in the first column)
std::cout << "Result: " << result << std::endl;

// Take care! Multiplicative depth and boostrapping is about nested calculations!

// Suggestion 1: track the level with the maximum threshold configured to trigger bootstrapping
// Suggestion 2: pack the values as most as possible

// To put in the paper
// Formula of CKKS
// The parameters (ring dimension, modulus, scaling factor)
// Explain SIMD

// Criptografia em Batch Learning - KNN / (Naive Bayes ou Perceptron)
// Algoritmos em streams

*/