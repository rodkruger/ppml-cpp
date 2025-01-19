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
        holdoutVal.Split(this->trainingRatio);

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

        this->Info("Encrypt testing data");

        auto eTestingData = ckksClient.EncryptCKKS(holdoutVal.GetTestingFeatures());
        auto eTestingLabels = ckksClient.EncryptCKKS(holdoutVal.GetLabels());

        end = std::chrono::high_resolution_clock::now();
        this->encryptingTime = end - start;

        this->Info("Elapsed time: " + std::to_string(this->encryptingTime.count()) + " ms");

        // S E R V E R   S I D E   P R O C E S S I N G --------------------------------------------------------------------

        this->Info(">>>>> SERVER SIDE PROCESSING");

        auto clf = CkksPerceptron(ckksCtx, features[0].size(), this->epochs);

        // Step 06 - Train the model

        this->Info("Train model");

        start = std::chrono::high_resolution_clock::now();

        clf.Fit(eTrainingData, eTrainingLabels);

        end = std::chrono::high_resolution_clock::now();
        this->trainingTime = end - start;

        this->Info("Elapsed time: " + std::to_string(this->trainingTime.count()) + " ms");

        // Step 07 - Test the model

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
        std::ofstream predictionsFile(predictionsFileName);

        if (!predictionsFile) {
            this->Error("Could not open the file " + predictionsFileName + " for writing.\n");
            return;
        }

        // Write the data to the file
        for (const auto &[realValue, predictedValue]: predictData) {
            predictionsFile << realValue << "," << predictedValue << std::endl;
        }

        // Close the file
        predictionsFile.close();

        end = std::chrono::high_resolution_clock::now();
        this->testingTime = end - start;

        this->Info("Elapsed time: " + std::to_string(this->testingTime.count()) + " ms");

        //-------------------------------------------------------------------------------------------------------------

        // Dump parameters into file

        auto parametersFileName = this->BuildFilePath("parameters.csv");
        std::ofstream parametersFile(parametersFileName);

        if (!parametersFile) {
            this->Error("Could not open the file " + predictionsFileName + " for writing.\n");
            return;
        }

        this->datasetLength = features.size();
        this->trainingLength = holdoutVal.GetTrainingFeatures().size();
        this->testingLength = holdoutVal.GetTestingFeatures().size();
        this->ringDimension = cc->GetRingDimension();
        this->multiplicativeDepth = ckksCtx.GetMultiplicativeDepth();

        // Write the data to the file
        parametersFile << "epochs = " << this->epochs << std::endl;
        parametersFile << "datasetLength = " << this->datasetLength << std::endl;
        parametersFile << "trainingRatio = " << this->trainingRatio << std::endl;
        parametersFile << "trainingLength = " << this->trainingLength << std::endl;
        parametersFile << "testingLength = " << this->testingLength << std::endl;
        parametersFile << "ringDimension = " << this->ringDimension << std::endl;
        parametersFile << "multiplicativeDepth = " << this->multiplicativeDepth << std::endl;
        parametersFile << "encryptingTime = " << std::to_string(this->encryptingTime.count()) << std::endl;
        parametersFile << "trainingTime = " << std::to_string(this->trainingTime.count()) << std::endl;
        parametersFile << "testingTime = " << std::to_string(this->trainingTime.count()) << std::endl;

        // Close the file
        parametersFile.close();
    }
}
