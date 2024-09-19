#include <iostream>
#include <vector>

#include "csv.h"
#include "openfhe.h"
#include "HermesMl.h"

using namespace lbcrypto;

namespace HermesMl {

    HEConfig::HEConfig() : HEConfig(65537, 2) {}

    HEConfig::HEConfig(int64_t modulus, int64_t multiplicativeDepth) {
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
        this->cc->EvalBootstrapKeyGen(keys.secretKey, this->modulus); // Generate bootstrapping keys
    }

    CryptoContext<DCRTPoly> HEConfig::getCc() const { return this->cc; }

    KeyPair<DCRTPoly> HEConfig::getKeyPair() const { return this->keys; }

    std::vector<std::vector<Ciphertext<DCRTPoly> > > HEConfig::encrypt(const std::vector<std::vector<int64_t>>& data) {
        auto encryptedData = std::vector<std::vector<Ciphertext<DCRTPoly>>>();

        for (const auto &point : data) {
            auto encryptedPoint = std::vector<Ciphertext<DCRTPoly>>();

            for (const auto value : point) {
                Plaintext plaintextValue = this->cc->MakePackedPlaintext({value});
                Ciphertext<DCRTPoly> encryptedValue = this->cc->Encrypt(this->keys.publicKey, plaintextValue);
                encryptedPoint.push_back(encryptedValue);
            }

            encryptedData.push_back(encryptedPoint);
        }

        return encryptedData;
    }

    KNeighboursClassifier::KNeighboursClassifier(int64_t k, const HEConfig& heConfig) : k(k), heConfig(heConfig) {}

    Ciphertext<DCRTPoly> KNeighboursClassifier::manhattan(const std::vector<Ciphertext<DCRTPoly> >& point1,
                                                                     const std::vector<Ciphertext<DCRTPoly> >& point2) {

        Plaintext plaintextZero = this->heConfig.getCc()->MakePackedPlaintext(std::vector<int64_t>{0});
        Ciphertext<DCRTPoly> distance = this->heConfig.getCc()->Encrypt(this->heConfig.getKeyPair().publicKey, plaintextZero);

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

    Ciphertext<DCRTPoly> KNeighboursClassifier::distance(const std::vector<Ciphertext<DCRTPoly> >& point1,
                                                                    const std::vector<Ciphertext<DCRTPoly> >& point2) {
        return this->manhattan(point1, point2);
    }

    void KNeighboursClassifier::fit(const std::vector<std::vector<Ciphertext<DCRTPoly>>>& trainingData,
             const std::vector<int64_t>& trainingLabels) {
        this->trainingData = trainingData;
        this->trainingLabels = trainingLabels;
    }

    int64_t KNeighboursClassifier::predict(const std::vector<Ciphertext<DCRTPoly>>& testingData) {

        // Step 1: Compute distances from the test point to all training points
        size_t numTrainingPoints = trainingData.size();
        std::vector<std::pair<Ciphertext<DCRTPoly>, int64_t>> distances;

        for (size_t i = 0; i < numTrainingPoints; ++i) {
            auto distance = this->distance(this->trainingData[i], testingData);
            distances.emplace_back(distance, this->trainingLabels[i]);
        }

        // Step 2: Sort the distances (may this sort needs to be implemented homomorphically!)
        sort(distances.begin(), distances.end());

        // Step 3: Iterate over the k-nearest neighbors and count their labels
        std::map<int64_t, int64_t> labelCount;

        for (int i = 0; i < k; ++i) {
            int64_t label = distances[i].second;
            labelCount[label]++;
        }

        // Step 4: Find the label with the highest count (majority voting)
        int64_t predictedLabel = -1;
        int64_t maxCount = 0;

        for (const auto& label : labelCount) {
            if (label.second > maxCount) {
                maxCount = label.second;
                predictedLabel = label.first;
            }
        }

        return predictedLabel;
    }
}

int main() {

    //-----------------------------------------------------------------------------------------------------------------
    // Read wine database

    std::vector<std::vector<double>> wineData;

    io::CSVReader<14> in("../datasets/wine/wine.data");

    int classLabel;
    double alcohol, malicAcid, ash, alcalinityOfAsh, magnesium, totalPhenols, flavanoids,
           nonflavonoidPhenols, proanthocyanins, colorIntensity, hue, odOfDilutedWines, proline;

    while (in.read_row(classLabel, alcohol, malicAcid, ash, alcalinityOfAsh, magnesium,
        totalPhenols, flavanoids, nonflavonoidPhenols, proanthocyanins, colorIntensity, hue,
        odOfDilutedWines, proline)) {

        std::vector<double> row = {alcohol, malicAcid, ash, alcalinityOfAsh, magnesium, totalPhenols,
            flavanoids, nonflavonoidPhenols, proanthocyanins, colorIntensity, hue,
            odOfDilutedWines, proline};

        wineData.push_back(row);
    }

    //-----------------------------------------------------------------------------------------------------------------
    // Split dataset in training and test. Holdout (70% training; 30% testing)

    int64_t trainingLength = wineData.size() * 0.7;
    int64_t testingLength = wineData.size() - trainingLength;

    std::vector<std::vector<double>> trainingData = std::vector<std::vector<double>>();
    std::vector<std::vector<double>> testingData = std::vector<std::vector<double>>();

    trainingData.reserve(trainingLength);
    testingData.reserve(testingLength);

    std::copy(wineData.begin(), wineData.begin() + trainingLength, std::back_inserter(trainingData));
    std::copy(wineData.begin() + trainingLength, wineData.end(), std::back_inserter(testingData));

    std::cout << "Training length: " << trainingData.size() << std::endl;
    std::cout << "Testing length: " << testingData.size() << std::endl;

    return 0;

    /**
    HermesMl::HEConfig config = HermesMl::HEConfig();

    std::vector<int64_t> trainingLabels = { 0, 1, 0 };

    std::vector<std::vector<int64_t>> testingData = { {2, 3} };

    std::vector<std::vector<Ciphertext<DCRTPoly> > > encryptedTrainingData = config.encrypt(trainingData);
    std::vector<std::vector<Ciphertext<DCRTPoly> > > encryptedTestingData = config.encrypt(testingData);

    HermesMl::KNeighboursClassifier model = HermesMl::KNeighboursClassifier(2, config );
    model.fit(encryptedTrainingData, trainingLabels);
    int64_t predictedLabel = model.predict(encryptedTestingData[0]);

    std::cout << "Predicted label: " << predictedLabel << std::endl;
    std::cout << "Done!" << std::endl;

    //TODO: put a larger dataset to test the algorithm
    //TODO: check the need for bootstrapping

    return 0;
    */
}