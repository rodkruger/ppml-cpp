#include "client.h"
#include "datasets.h"
#include "experiments.h"
#include "model.h"

namespace hermesml {
    CkksPerceptronExperiment::CkksPerceptronExperiment(const std::string &experimentId,
                                                       const CkksPerceptronExperimentParams &params) : Experiment(
            experimentId),
        params(params) {
    }

    void CkksPerceptronExperiment::Run() {
        std::chrono::time_point<std::chrono::system_clock> start, end;

        this->Info("Initiating experiment " + this->GetExperimentId());
        this->Info(">>>>> CLIENT SIDE PROCESSING");

        // Step 01 - read and normalize data
        this->Info("Read dataset");;

        BreastCancerDataset::BreastCancerDatasetRanges datasetType;

        switch (this->params.activation) {
            case CkksPerceptron::TANH:
                datasetType = BreastCancerDataset::BreastCancerDatasetRanges::F11;
                break;

            case CkksPerceptron::SIGMOID:
                datasetType = BreastCancerDataset::BreastCancerDatasetRanges::F11;
                break;

            default:
                datasetType = BreastCancerDataset::BreastCancerDatasetRanges::F01;
                break;
        }

        const auto trainingFeatures = BreastCancerDataset::GetTrainingFeatures(datasetType);
        const auto trainingLabels = BreastCancerDataset::GetTrainingLabels(datasetType);
        const auto testingFeatures = BreastCancerDataset::GetTestingFeatures(datasetType);
        const auto testingLabels = BreastCancerDataset::GetTestingLabels(datasetType);

        this->Info("Total samples: " + std::to_string(trainingFeatures.size() + testingFeatures.size()));
        this->Info("Training length: " + std::to_string(trainingFeatures.size()));
        this->Info("Testing length: " + std::to_string(testingFeatures.size()));

        if (trainingFeatures.size() != trainingLabels.size()) {
            throw std::runtime_error("Wrong number of training features and labels provided!");
        }

        if (testingFeatures.size() != testingLabels.size()) {
            throw std::runtime_error("Wrong number of testing features and labels provided!");
        }

        //-----------------------------------------------------------------------------------------------------------------

        // Step 02 - Generating keys and crypto context. Do not share it with anybody :)

        this->Info("Generate crypto context");

        auto ckksCtx = HEContextFactory::ckksHeContext();
        ckksCtx.SetEarlyBootstrapping(this->params.earlyBootstrapping);

        auto ckksClient = Client(ckksCtx);
        auto cc = ckksCtx.GetCc();

        this->Info("Scheme: CKKS");
        this->Info("Activation: " + std::to_string(this->params.activation));
        this->Info("Ring dimension: " + std::to_string(cc->GetRingDimension()));
        this->Info("Scaling Modulus Size: " + std::to_string(ckksCtx.GetScalingModSize()));
        this->Info("Modulus: " + cc->GetModulus().ToString());
        this->Info("Multiplicative depth: " + std::to_string(ckksCtx.GetMultiplicativeDepth()));
        this->Info("Early Boostrapping: " + std::to_string(ckksCtx.GetEarlyBootstrapping()));

        //-----------------------------------------------------------------------------------------------------------------

        // Step 03 - Encrypt training data

        this->Info("Encrypt training data");

        start = std::chrono::high_resolution_clock::now();

        auto eTrainingData = ckksClient.EncryptCKKS(trainingFeatures);
        auto eTrainingLabels = ckksClient.EncryptCKKS(trainingLabels, trainingFeatures[0].size());

        this->Info("Encrypt testing data");

        auto eTestingData = ckksClient.EncryptCKKS(testingFeatures);
        auto eTestingLabels = ckksClient.EncryptCKKS(testingLabels, testingFeatures[0].size());

        if (eTrainingData.size() != eTrainingLabels.size()) {
            throw std::runtime_error("Wrong number of encrypted training features and labels provided!");
        }

        if (eTestingData.size() != eTestingLabels.size()) {
            throw std::runtime_error("Wrong number of encrypted testing features and labels provided!");
        }

        end = std::chrono::high_resolution_clock::now();
        this->encryptingTime = end - start;

        this->Info("Elapsed time: " + std::to_string(this->encryptingTime.count()) + " ms");

        // S E R V E R   S I D E   P R O C E S S I N G --------------------------------------------------------------------

        this->Info(">>>>> SERVER SIDE PROCESSING");

        auto clf = CkksPerceptron(ckksCtx, trainingFeatures[0].size(), this->params.epochs, this->params.activation);

        // Step 04 - Train the model

        this->Info("Train model");

        start = std::chrono::high_resolution_clock::now();

        clf.Fit(eTrainingData, eTrainingLabels);

        end = std::chrono::high_resolution_clock::now();
        this->trainingTime = end - start;

        this->Info("Elapsed time: " + std::to_string(this->trainingTime.count()) + " ms");

        // Step 05 - Test the model

        this->Info("Test model");

        // Open the file in write mode
        auto predictionsFileName = this->BuildFilePath("predictions.csv");

        if (std::filesystem::exists(predictionsFileName)) {
            std::filesystem::remove(predictionsFileName);
        }

        std::ofstream predictionsFile(predictionsFileName);

        if (!predictionsFile) {
            this->Error("Could not open the file " + predictionsFileName + " for writing.\n");
            return;
        }

        start = std::chrono::high_resolution_clock::now();

        Plaintext plain_label;

        for (size_t i = 0; i < eTestingData.size(); i++) {
            const auto &eFeatures = eTestingData[i];
            const auto ePredictedLabel = clf.Predict(eFeatures).GetCiphertext();
            cc->Decrypt(ckksCtx.GetPrivateKey(), ePredictedLabel, &plain_label);
            const auto pPrediction = plain_label->GetCKKSPackedValue()[0].real();

            double pPredictedLabel = 0.0;
            switch (this->params.activation) {
                case CkksPerceptron::TANH:
                    pPredictedLabel = pPrediction > 0.0 ? 1.0 : -1.0;
                    break;

                case CkksPerceptron::SIGMOID:
                    pPredictedLabel = pPrediction > 0.5 ? 1.0 : -1.0;
                    break;

                default:
                    pPredictedLabel = pPrediction > 0.5 ? 1.0 : -1.0;
                    break;
            }

            const auto realLabel = testingLabels[i];

            predictionsFile << pPrediction << "," << realLabel << "," << pPredictedLabel << std::endl;
        }

        // Close the file
        predictionsFile.close();

        end = std::chrono::high_resolution_clock::now();
        this->testingTime = end - start;

        this->Info("Elapsed time: " + std::to_string(this->testingTime.count()) + " ms");

        //-------------------------------------------------------------------------------------------------------------

        // Dump parameters into file

        auto parametersFileName = this->BuildFilePath("parameters.csv");

        if (std::filesystem::exists(parametersFileName)) {
            std::filesystem::remove(parametersFileName);
        }

        std::ofstream parametersFile(parametersFileName);

        if (!parametersFile) {
            this->Error("Could not open the file " + predictionsFileName + " for writing.\n");
            return;
        }

        this->datasetLength = trainingFeatures.size() + testingFeatures.size();
        this->trainingLength = trainingFeatures.size();
        this->testingLength = testingFeatures.size();
        this->ringDimension = cc->GetRingDimension();
        this->multiplicativeDepth = ckksCtx.GetMultiplicativeDepth();

        // Write the data to the file
        parametersFile << "epochs = " << this->params.epochs << std::endl;
        parametersFile << "datasetLength = " << this->datasetLength << std::endl;
        parametersFile << "trainingRatio = " << this->trainingRatio << std::endl;
        parametersFile << "trainingLength = " << this->trainingLength << std::endl;
        parametersFile << "testingLength = " << this->testingLength << std::endl;
        parametersFile << "ringDimension = " << this->ringDimension << std::endl;
        parametersFile << "multiplicativeDepth = " << std::to_string(this->multiplicativeDepth) << std::endl;
        parametersFile << "encryptingTime = " << std::to_string(this->encryptingTime.count()) << std::endl;
        parametersFile << "trainingTime = " << std::to_string(this->trainingTime.count()) << std::endl;
        parametersFile << "testingTime = " << std::to_string(this->testingTime.count()) << std::endl;

        // Close the file
        parametersFile.close();

        this->Info("Experiment " + this->GetExperimentId() + " completed!");
    }
}
