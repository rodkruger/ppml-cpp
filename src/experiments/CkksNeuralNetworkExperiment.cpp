#include "client.h"
#include "datasets.h"
#include "experiments.h"
#include "model.h"

#include <filesystem>

namespace fs = std::filesystem;

namespace hermesml {
    CkksNeuralNetworkExperiment::CkksNeuralNetworkExperiment(const std::string &experimentId,
                                                             Dataset &dataset,
                                                             const CkksExperimentParams &
                                                             params) : Experiment(
                                                                           experimentId, dataset),
                                                                       params(params) {
    }

    void CkksNeuralNetworkExperiment::Run() {
        RunMemory();
    }

    void CkksNeuralNetworkExperiment::RunMemory() {
        std::chrono::time_point<std::chrono::system_clock> start, end;

        const auto predictionsPath = std::filesystem::current_path() / "Predictions" / this->GetDataset().GetName() /
                                     this->GetExperimentId();

        if (exists(predictionsPath) && is_directory(predictionsPath)) {
            if (std::filesystem::directory_iterator(predictionsPath) != std::filesystem::end(
                    std::filesystem::directory_iterator())) {
                this->Info("Experiment " + this->GetExperimentId() + " already executed. Ignoring!");
                return;
            }
        }

        this->Info(">>>>> CLIENT SIDE PROCESSING");

        // Step 01 - Read and normalize data --------------------------------------------------------------------------
        this->Info("Read dataset " + this->GetDataset().GetName());;

        const auto trainingFeatures = this->GetDataset().GetTrainingFeatures();
        const auto trainingLabels = this->GetDataset().GetTrainingLabels();
        const auto testingFeatures = this->GetDataset().GetTestingFeatures();
        const auto testingLabels = this->GetDataset().GetTestingLabels();

        /* Use it only for debugging purpose
        const std::vector<std::vector<double> > trainingFeatures = {{1.0, 2.0, 3.0}};
        const std::vector<double> trainingLabels = {1.0};
        const std::vector<std::vector<double> > testingFeatures = {{1.0, 2.0, 3.0}};
        const std::vector<double> testingLabels = {1.0};
        /* */

        const auto n_features = trainingFeatures[0].size();

        this->Info("Total samples: " + std::to_string(trainingFeatures.size() + testingFeatures.size()));
        this->Info("Training length: " + std::to_string(trainingFeatures.size()));
        this->Info("Testing length: " + std::to_string(testingFeatures.size()));
        this->Info("Number of features: " + std::to_string(n_features));

        if (trainingFeatures.size() != trainingLabels.size()) {
            throw std::runtime_error("Wrong number of training features and labels provided!");
        }

        if (testingFeatures.size() != testingLabels.size()) {
            throw std::runtime_error("Wrong number of testing features and labels provided!");
        }

        // Step 02 - Generating keys and crypto context. Do not share it with anybody :) ------------------------------
        this->Info("Generate crypto context");

        auto ckksCtx = HEContextFactory::ckksHeContext(n_features);
        ckksCtx.SetEarlyBootstrapping(this->params.earlyBootstrapping);

        auto ckksClient = Client(ckksCtx);
        auto cc = ckksCtx.GetCc();

        this->Info("Scheme: CKKS");
        this->Info("Activation: " + std::to_string(this->params.activation));
        this->Info("Approximation: " + std::to_string(this->params.approximation));
        this->Info("Ring dimension: " + std::to_string(cc->GetRingDimension()));
        this->Info("Scaling Modulus Size: " + std::to_string(ckksCtx.GetScalingModSize()));
        this->Info("Modulus: " + cc->GetModulus().ToString());
        this->Info("Multiplicative depth: " + std::to_string(ckksCtx.GetMultiplicativeDepth()));
        this->Info("Early Boostrapping: " + std::to_string(ckksCtx.GetEarlyBootstrapping()));
        this->Info("Number of Slots: " + std::to_string(ckksCtx.GetNumSlots()));

        // Step 03 - Encrypt data -------------------------------------------------------------------------------------
        start = std::chrono::high_resolution_clock::now();

        std::vector<BootstrapableCiphertext> eTrainingData, eTrainingLabels, eTestingData, eTestingLabels;

        this->Info("Encrypting training data");
        eTrainingData = ckksClient.EncryptCKKS(trainingFeatures);
        eTrainingLabels = ckksClient.EncryptCKKS(trainingLabels, trainingFeatures[0].size());
        eTestingData = ckksClient.EncryptCKKS(testingFeatures);
        eTestingLabels = ckksClient.EncryptCKKS(testingLabels, testingFeatures[0].size());

        if (eTrainingData.size() != eTrainingLabels.size()) {
            throw std::runtime_error("Wrong number of encrypted training features and labels provided!");
        }

        if (eTestingData.size() != eTestingLabels.size()) {
            throw std::runtime_error("Wrong number of encrypted testing features and labels provided!");
        }

        end = std::chrono::high_resolution_clock::now();
        this->encryptingTime = end - start;

        this->Info("Elapsed time: " + std::to_string(this->encryptingTime.count()) + " ms");

        // S E R V E R   S I D E   P R O C E S S I N G ----------------------------------------------------------------
        this->Info(">>>>> SERVER SIDE PROCESSING");

        std::vector<size_t> layers = {n_features, 10, 5, 1};
        auto clf = CkksNeuralNetwork(ckksCtx, n_features, params.epochs, layers, 42, params.activation,
                                     params.approximation);

        // Step 04 - Train the model ----------------------------------------------------------------------------------
        auto experimentId = this->GetExperimentId();

        for (int e = 1; e <= this->params.epochs; e++) {
            this->SetExperimentId(experimentId + "_" + std::to_string(e));

            auto predictionsFileName = this->BuildFilePath("predictions.csv");

            if (std::filesystem::exists(predictionsFileName)) {
                std::ifstream existingPredictionFile(predictionsFileName);
                size_t lineCount = 0;
                std::string line;

                while (std::getline(existingPredictionFile, line)) {
                    lineCount++;
                }

                if (lineCount == eTestingData.size()) {
                    this->Info("Experiment " + this->GetExperimentId() + " is done. Skipped!");
                    return;
                }
            }

            std::ofstream predictionsFile(predictionsFileName);
            if (!predictionsFile) {
                this->Error("Could not open the file " + predictionsFileName + " for writing. Skipped!");
                return;
            }

            // Step 04 - Train the model ------------------------------------------------------------------------------
            this->Info("Initiating experiment " + this->GetExperimentId());
            this->Info("Train model");

            start = std::chrono::high_resolution_clock::now();

            clf.FitSingle(eTrainingData, eTrainingLabels);

            end = std::chrono::high_resolution_clock::now();
            this->trainingTime += end - start;

            this->Info("Elapsed time: " + std::to_string(this->trainingTime.count()) + " ms");

            // Step 05 - Test the model - -----------------------------------------------------------------------------
            this->Info("Test model");

            start = std::chrono::high_resolution_clock::now();

            Plaintext plain_label;

            for (size_t i = 0; i < eTestingData.size(); i++) {
                const auto &eFeatures = eTestingData[i];
                const auto ePredictedLabel = clf.Predict(eFeatures).GetCiphertext();
                cc->Decrypt(ckksCtx.GetPrivateKey(), ePredictedLabel, &plain_label);
                const auto pPrediction = plain_label->GetCKKSPackedValue()[0].real();

                double pPredictedLabel = 0.0;
                switch (this->params.activation) {
                    case TANH:
                        pPredictedLabel = pPrediction > 0.0 ? 1.0 : 0.0;
                        break;

                    case SIGMOID:
                        pPredictedLabel = pPrediction > 0.5 ? 1.0 : 0.0;
                        break;
                }

                const auto realLabel = testingLabels[i];

                predictionsFile << pPrediction << "," << realLabel << "," << pPredictedLabel << std::endl;
            }

            predictionsFile.close();

            end = std::chrono::high_resolution_clock::now();
            this->testingTime = end - start;

            this->Info("Elapsed time: " + std::to_string(this->testingTime.count()) + " ms");

            //---------------------------------------------------------------------------------------------------------

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

    void CkksNeuralNetworkExperiment::RunHardDisk() {
        std::chrono::time_point<std::chrono::system_clock> start, end;

        const auto predictionsPath = std::filesystem::current_path() / "Predictions" / this->GetDataset().GetName() /
                                     this->GetExperimentId();

        if (exists(predictionsPath) && is_directory(predictionsPath)) {
            if (std::filesystem::directory_iterator(predictionsPath) != std::filesystem::end(
                    std::filesystem::directory_iterator())) {
                this->Info("Experiment " + this->GetExperimentId() + " already executed. Ignoring!");
                return;
            }
        }

        this->Info("Initiating experiment " + this->GetExperimentId());
        this->Info(">>>>> CLIENT SIDE PROCESSING");

        // Step 01 - read and normalize data
        this->Info("Read dataset " + this->GetDataset().GetName());;

        const auto trainingFeatures = this->GetDataset().GetTrainingFeatures();
        const auto trainingLabels = this->GetDataset().GetTrainingLabels();
        const auto testingFeatures = this->GetDataset().GetTestingFeatures();
        const auto testingLabels = this->GetDataset().GetTestingLabels();
        const auto n_features = trainingFeatures[0].size();

        this->Info("Total samples: " + std::to_string(trainingFeatures.size() + testingFeatures.size()));
        this->Info("Training length: " + std::to_string(trainingFeatures.size()));
        this->Info("Testing length: " + std::to_string(testingFeatures.size()));
        this->Info("Number of features: " + std::to_string(n_features));

        if (trainingFeatures.size() != trainingLabels.size()) {
            throw std::runtime_error("Wrong number of training features and labels provided!");
        }

        if (testingFeatures.size() != testingLabels.size()) {
            throw std::runtime_error("Wrong number of testing features and labels provided!");
        }

        //-----------------------------------------------------------------------------------------------------------------

        // Step 02 - Generating keys and crypto context. Do not share it with anybody :)

        this->Info("Generate crypto context");

        auto ckksCtx = HEContextFactory::ckksHeContext(n_features);
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
        this->Info("Number of Slots: " + std::to_string(ckksCtx.GetNumSlots()));

        //-----------------------------------------------------------------------------------------------------------------

        const auto eTrainingFeaturesFilePath = this->GetDataset().GetContentPath() + "/encrypted_training_features.bin";
        const auto eTrainingLabelsFilePath = this->GetDataset().GetContentPath() + "/encrypted_training_labels.bin";
        const auto eTestingFeaturesFilePath = this->GetDataset().GetContentPath() + "/encrypted_testing_features.bin";
        const auto eTestingLabelsFilePath = this->GetDataset().GetContentPath() + "/encrypted_testing_labels.bin";

        // Step 03 - Encrypt training data

        this->Info("Encrypt training data");

        start = std::chrono::high_resolution_clock::now();

        ckksClient.EncryptCKKS(trainingFeatures, eTrainingFeaturesFilePath);
        ckksClient.EncryptCKKS(trainingLabels, trainingFeatures[0].size(), eTrainingLabelsFilePath);

        this->Info("Encrypt testing data");
        ckksClient.EncryptCKKS(testingFeatures, eTestingFeaturesFilePath);
        ckksClient.EncryptCKKS(testingLabels, testingFeatures[0].size(), eTestingLabelsFilePath);

        end = std::chrono::high_resolution_clock::now();

        this->encryptingTime = end - start;

        this->Info("Elapsed time: " + std::to_string(this->encryptingTime.count()) + " ms");

        // S E R V E R   S I D E   P R O C E S S I N G --------------------------------------------------------------------

        this->Info(">>>>> SERVER SIDE PROCESSING");

        std::vector<size_t> layers = {2, 3, 1};
        auto clf = CkksNeuralNetwork(ckksCtx, n_features, params.epochs, layers, 42, params.activation,
                                     params.approximation);

        // Step 04 - Train the model

        this->Info("Train model");

        start = std::chrono::high_resolution_clock::now();

        clf.Fit(eTrainingFeaturesFilePath, eTrainingLabelsFilePath);

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

        auto i = 0;
        const auto ePredictions = clf.PredictAll(eTestingFeaturesFilePath);

        for (const auto &ePrediction: ePredictions) {
            Plaintext plain_label;
            cc->Decrypt(ckksCtx.GetPrivateKey(), ePrediction.GetCiphertext(), &plain_label);
            const auto pPrediction = plain_label->GetCKKSPackedValue()[0].real();

            double pPredictedLabel = 0.0;
            switch (this->params.activation) {
                case TANH:
                    pPredictedLabel = pPrediction > 0.0 ? 1.0 : 0.0;
                    break;

                case SIGMOID:
                    pPredictedLabel = pPrediction > 0.5 ? 1.0 : 0.0;
                    break;

                default:
                    pPredictedLabel = pPrediction > 0.5 ? 1.0 : 0.0;
                    break;
            }

            const auto realLabel = testingLabels[i];
            predictionsFile << pPrediction << "," << realLabel << "," << pPredictedLabel << std::endl;

            i++;
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

        /*
        std::remove(eTrainingFeaturesFilePath.c_str());
        std::remove(eTrainingLabelsFilePath.c_str());
        std::remove(eTestingFeaturesFilePath.c_str());
        std::remove(eTestingLabelsFilePath.c_str());
        */

        this->Info("Experiment " + this->GetExperimentId() + " completed!");
    }
}
