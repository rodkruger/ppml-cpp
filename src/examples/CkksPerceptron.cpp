#include <iostream>
#include "csv.hpp"
#include "model.h"
#include "client.h"
#include "datasets.h"
#include "validation.h"

using namespace hermesml;

int main() {
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed{};

    std::cout << ">>> CLIENT SIDE PROCESSING" << std::endl;

    // Step 01 - read and normalize data
    std::cout << "# Read dataset " << std::endl;
    auto datasetHandler = BreastCancerDataset();
    auto features = datasetHandler.GetFeatures();
    auto labels = datasetHandler.GetLabels();

    MinMaxScaler::Scale(features);

    // Step 02 - split dataset in training and testing. Holdout (70% training; 30% testing)
    std::cout << "# Split dataset (70% training; 30% testing)" << std::endl;
    auto holdoutVal = Holdout(features, labels);
    holdoutVal.Split();

    std::cout << "      Total samples: " << (features.size()) << std::endl;
    std::cout << "      Training length: " << holdoutVal.GetTrainingFeatures().size() << std::endl;
    std::cout << "      Testing length: " << holdoutVal.GetTestingFeatures().size() << std::endl;

    //-----------------------------------------------------------------------------------------------------------------

    // Step 03
    // Generating keys and crypto context. Do not share it with anybody :)

    std::cout << "# Generate crypto context" << std::endl;

    auto ckksCtx = HEContextFactory::ckksHeContext();
    auto ckksClient = Client(ckksCtx);
    auto cc = ckksCtx.GetCc();

    std::cout << "      Scheme: " << cc->GetScheme() << std::endl;
    std::cout << "      Ring dimension: " << cc->GetRingDimension() << std::endl;
    std::cout << "      Modulus: " << cc->GetModulus() << std::endl;
    std::cout << "      Multiplicative depth: " << ckksCtx.GetMultiplicativeDepth() << std::endl;

    //-----------------------------------------------------------------------------------------------------------------

    // Step 04
    // Encrypt training data

    std::cout << "# Encrypt training data ... " << std::flush;

    start = std::chrono::high_resolution_clock::now();

    auto eTrainingData = ckksClient.EncryptCKKS(holdoutVal.GetTrainingFeatures());
    auto eTrainingLabels = ckksClient.EncryptCKKS(holdoutVal.GetLabels());

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

    //-----------------------------------------------------------------------------------------------------------------

    // Step 05
    // Encrypt testing data

    std::cout << "# Encrypt testing data ... " << std::flush;

    start = std::chrono::high_resolution_clock::now();

    auto eTestingData = ckksClient.EncryptCKKS(holdoutVal.GetTestingFeatures());
    auto eTestingLabels = ckksClient.EncryptCKKS(holdoutVal.GetLabels());

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

    // S E R V E R   S I D E   P R O C E S S I N G --------------------------------------------------------------------

    std::cout << std::endl;
    std::cout << ">>> SERVER SIDE PROCESSING" << std::endl;

    auto clf = CkksPerceptron(ckksCtx, features[0].size(), 1);

    std::cout << "# Train model ... " << std::flush;

    start = std::chrono::high_resolution_clock::now();

    clf.Fit(eTrainingData, eTrainingLabels);

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

    std::cout << "# Test model ... " << std::flush;

    start = std::chrono::high_resolution_clock::now();

    Plaintext plain_label;
    int32_t correct_predictions = 0;

    for (uint32_t i = 0; i < eTestingData.size(); i++) {
        auto ePredictedLabel = clf.Predict(eTestingData[i]).GetCiphertext();
        cc->Decrypt(ckksCtx.GetPrivateKey(), ePredictedLabel, &plain_label);
        auto pPredictedLabel = plain_label->GetCKKSPackedValue()[0].real() > 0.5 ? 1.0 : 0.0;
        auto realLabel = holdoutVal.GetTestingLabels()[i];
        std::cout << "# Real label: " << realLabel << ". Decrypted label: " << pPredictedLabel << std::endl;

        if (pPredictedLabel == realLabel) {
            correct_predictions++;
        }
    }

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;
    std::cout << "Accuracy: " << (correct_predictions / holdoutVal.GetTestingLabels().size()) << std::endl;

    // 420.000 -> 17 hours / 1020 minutes
    // 200 -> x = 0,48 (appr. 30 seconds)

    // Multiplicative Depth: 20 (minimum value)
    // Encryption Time: 60 seconds
    // Training Time: 950 seconds

    return EXIT_SUCCESS;
}
