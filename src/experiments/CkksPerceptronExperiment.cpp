#include "client.h"
#include "datasets.h"
#include "experiments.h"
#include "model.h"
#include "validation.h"

namespace hermesml {
    CkksPerceptronExperiment::CkksPerceptronExperiment(const std::string &experimentId,
                                                       const uint16_t epochs) : Experiment(experimentId),
        epochs(epochs) {
    }

    void CkksPerceptronExperiment::Run() {
        std::chrono::time_point<std::chrono::system_clock> start, end;
        std::chrono::duration<double> elapsed{};

        this->Info(">>>>> CLIENT SIDE PROCESSING");

        spdlog::shutdown();

        // Step 01 - read and normalize data
        this->Info("Read dataset");;
        const auto datasetHandler = BreastCancerDataset();
        auto features = datasetHandler.GetFeatures();
        auto labels = datasetHandler.GetLabels();

        // Step 02 - read and normalize data
        this->Info("Normalize dataset");;
        MinMaxScaler::Scale(features);

        // Step 03 - split dataset in training and testing. Holdout (70% training; 30% testing)
        this->Info("Split dataset (70% training; 30% testing)");

        auto holdoutVal = Holdout(features, labels);
        holdoutVal.Split(0.7);

        this->Info("Total samples: " + std::to_string(features.size()));
        this->Info("Training length: " + std::to_string(holdoutVal.GetTrainingFeatures().size()));
        this->Info("Testing length: " + std::to_string(holdoutVal.GetTestingFeatures().size()));

        //-----------------------------------------------------------------------------------------------------------------

        // Step 04 - Generating keys and crypto context. Do not share it with anybody :)

        this->Info("Generate crypto context");

        auto ckksCtx = HEContextFactory::ckksHeContext();
        auto ckksClient = Client(ckksCtx);
        auto cc = ckksCtx.GetCc();

        this->Info("Scheme: CKKS");
        this->Info("Ring dimension: " + std::to_string(cc->GetRingDimension()));
        this->Info("Modulus: " + cc->GetModulus().ToString());
        this->Info("Multiplicative depth: " + std::to_string(ckksCtx.GetMultiplicativeDepth()));

        //-----------------------------------------------------------------------------------------------------------------

        // Step 05 - Encrypt training data

        this->Info("Encrypt training data");

        start = std::chrono::high_resolution_clock::now();

        auto eTrainingData = ckksClient.EncryptCKKS(holdoutVal.GetTrainingFeatures());
        auto eTrainingLabels = ckksClient.EncryptCKKS(holdoutVal.GetLabels());

        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;

        this->Info("Elapsed time: " + std::to_string(elapsed.count()) + " ms");

        //-----------------------------------------------------------------------------------------------------------------

        // Step 06 - Encrypt testing data

        this->Info("Encrypt testing data");

        start = std::chrono::high_resolution_clock::now();

        auto eTestingData = ckksClient.EncryptCKKS(holdoutVal.GetTestingFeatures());
        auto eTestingLabels = ckksClient.EncryptCKKS(holdoutVal.GetLabels());

        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;

        this->Info("Elapsed time: " + std::to_string(elapsed.count()) + " ms");

        // S E R V E R   S I D E   P R O C E S S I N G --------------------------------------------------------------------

        this->Info(">>>>> SERVER SIDE PROCESSING");

        auto clf = CkksPerceptron(ckksCtx, features[0].size(), this->epochs);

        // Step 07 - Train the model

        this->Info("Train model");

        start = std::chrono::high_resolution_clock::now();

        clf.Fit(eTrainingData, eTrainingLabels);

        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;

        this->Info("Elapsed time: " + std::to_string(elapsed.count()) + " ms");

        // Step 08 - Test the model

        this->Info("Test model");

        auto predictData = std::vector<std::pair<double, double> >(eTestingData.size());
        Plaintext plain_label;

        start = std::chrono::high_resolution_clock::now();

        for (size_t i = 0; i < eTestingData.size(); i++) {
            const auto &eTestingLabel = eTestingLabels[i];
            const auto ePredictedLabel = clf.Predict(eTestingLabel).GetCiphertext();
            cc->Decrypt(ckksCtx.GetPrivateKey(), ePredictedLabel, &plain_label);
            const auto pPlainLabel = plain_label->GetCKKSPackedValue()[0].real();
            const auto pPredictedLabel = pPlainLabel > 0.0 ? 1.0 : 0.0;

            const auto realLabel = holdoutVal.GetLabels()[i];

            const auto prediction = std::pair(realLabel, pPredictedLabel);
            predictData.push_back(prediction);
        }

        // Open the file in write mode
        auto predictionsFileName = this->BuildFilePath("predictions.csv");
        std::ofstream outFile(predictionsFileName);

        if (!outFile) {
            this->Error("Could not open the file " + predictionsFileName + " for writing.\n");
            return;
        }

        // Write the data to the file
        for (const auto &[realValue, predictedValue]: predictData) {
            outFile << realValue << "," << predictedValue << std::endl;
        }

        // Close the file
        outFile.close();

        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;

        this->Info("Elapsed time: " + std::to_string(elapsed.count()) + " ms");
    }
}
