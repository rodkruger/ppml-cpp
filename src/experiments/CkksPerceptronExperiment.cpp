#include "client.h"
#include "datasets.h"
#include "experiments.h"
#include "model.h"
#include "validation.h"

namespace hermesml {
    CkksPerceptronExperiment::CkksPerceptronExperiment(const std::string &experimentId) : Experiment(experimentId) {
    }

    void CkksPerceptronExperiment::run() {
        std::chrono::time_point<std::chrono::system_clock> start, end;
        std::chrono::duration<double> elapsed{};

        logger->info(">>>>> CLIENT SIDE PROCESSING");

        spdlog::shutdown();

        // Step 01 - read and normalize data
        logger->info("Read dataset");;
        const auto datasetHandler = BreastCancerDataset();
        auto features = datasetHandler.GetFeatures();
        auto labels = datasetHandler.GetLabels();

        // Step 02 - read and normalize data
        logger->info("Normalize dataset");;
        MinMaxScaler::Scale(features);

        // Step 03 - split dataset in training and testing. Holdout (70% training; 30% testing)
        logger->info("Split dataset (70% training; 30% testing)");

        auto holdoutVal = Holdout(features, labels);
        holdoutVal.Split(0.7);

        logger->info("Total samples: " + std::to_string(features.size()));
        logger->info("Training length: " + std::to_string(holdoutVal.GetTrainingFeatures().size()));
        logger->info("Testing length: " + std::to_string(holdoutVal.GetTestingFeatures().size()));

        //-----------------------------------------------------------------------------------------------------------------

        // Step 04 - Generating keys and crypto context. Do not share it with anybody :)

        logger->info("Generate crypto context");

        auto ckksCtx = HEContextFactory::ckksHeContext();
        auto ckksClient = Client(ckksCtx);
        auto cc = ckksCtx.GetCc();

        logger->info("Scheme: CKKS");
        logger->info("Ring dimension: " + std::to_string(cc->GetRingDimension()));
        logger->info("Modulus: " + cc->GetModulus().ToString());
        logger->info("Multiplicative depth: " + std::to_string(ckksCtx.GetMultiplicativeDepth()));

        //-----------------------------------------------------------------------------------------------------------------

        // Step 05 - Encrypt training data

        logger->info("Encrypt training data");

        start = std::chrono::high_resolution_clock::now();

        auto eTrainingData = ckksClient.EncryptCKKS(holdoutVal.GetTrainingFeatures());
        auto eTrainingLabels = ckksClient.EncryptCKKS(holdoutVal.GetLabels());

        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;

        logger->info("Elapsed time: " + std::to_string(elapsed.count()) + " ms");

        //-----------------------------------------------------------------------------------------------------------------

        // Step 06 - Encrypt testing data

        logger->info("Encrypt testing data");

        start = std::chrono::high_resolution_clock::now();

        auto eTestingData = ckksClient.EncryptCKKS(holdoutVal.GetTestingFeatures());
        auto eTestingLabels = ckksClient.EncryptCKKS(holdoutVal.GetLabels());

        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;

        logger->info("Elapsed time: " + std::to_string(elapsed.count()) + " ms");

        // S E R V E R   S I D E   P R O C E S S I N G --------------------------------------------------------------------

        logger->info(">>>>> SERVER SIDE PROCESSING");

        auto clf = CkksPerceptron(ckksCtx, features[0].size(), 1);

        // Step 07 - Train the model

        logger->info("Train model");

        start = std::chrono::high_resolution_clock::now();

        clf.Fit(eTrainingData, eTrainingLabels);

        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;

        logger->info("Elapsed time: " + std::to_string(elapsed.count()) + " ms");

        // Step 08 - Test the model

        logger->info("Test model");

        start = std::chrono::high_resolution_clock::now();

        Plaintext plain_label;
        int32_t correct_predictions = 0;

        for (uint32_t i = 0; i < eTestingData.size(); i++) {
            auto ePredictedLabel = clf.Predict(eTestingData[i]).GetCiphertext();
            cc->Decrypt(ckksCtx.GetPrivateKey(), ePredictedLabel, &plain_label);
            auto realLabel = holdoutVal.GetTestingLabels()[i];
            auto pPlainLabel = plain_label->GetCKKSPackedValue()[0].real();
            auto pPredictedLabel = pPlainLabel > 0.0 ? 1.0 : 0.0;

            if (pPredictedLabel == realLabel) {
                correct_predictions++;
            }
        }

        end = std::chrono::high_resolution_clock::now();
        elapsed = end - start;

        logger->info("Elapsed time: " + std::to_string(elapsed.count()) + " ms");

        //-----------------------------------------------------------------------------------------------------------------

        // Step 09 - Metrics

        logger->info("Metrics");
        logger->info("Correct predictions: " + std::to_string(correct_predictions));

        auto accuracy =
                static_cast<double>(correct_predictions) / static_cast<double>(holdoutVal.GetTestingLabels().size());

        logger->info("Accuracy: " + std::to_string(accuracy));
    }
}
