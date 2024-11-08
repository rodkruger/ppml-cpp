#include <iostream>

// #include "csv.h"
#include "model.h"
#include "client.h"

using namespace hermesml;

int main() {

    /**
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed;

    // C L I E N T   S I D E   P R O C E S S I N G --------------------------------------------------------------------

    std::cout << ">>> CLIENT SIDE PROCESSING" << std::endl;

    // Step 01
    // Read dataset

    std::cout << "# Read dataset " << std::endl;

    auto wineData = std::vector<std::vector<double>>();
    auto wineLabels = std::vector<int64_t>();

    int classLabel = 0;
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

    //-----------------------------------------------------------------------------------------------------------------

    // Step 02
    // Split dataset in training and testing. Holdout (70% training; 30% testing)

    std::cout << "# Split dataset (70% training; 30% testing)" << std::endl;

    int64_t trainingLength = wineData.size() * 0.7;
    int64_t testingLength = wineData.size() - trainingLength;

    auto trainingData = std::vector<std::vector<double>>();
    auto testingData = std::vector<std::vector<double>>();

    auto trainingLabels = std::vector<int32_t>();
    auto testingLabels = std::vector<int32_t>();

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

    auto ckksCtx = HEContextFactory().ckksHeContext();
    auto ckksClient = Client(ckksCtx);
    auto cc = ckksCtx.GetCc();

    std::cout << "      Ring dimension: " << cc->GetRingDimension() << std::endl;
    std::cout << "      Modulus: " << cc->GetModulus() << std::endl;
    std::cout << "      Multiplicative depth: " << ckksCtx.GetMultiplicativeDepth() << std::endl;

    //-----------------------------------------------------------------------------------------------------------------

    // Step 04
    // Encrypt training data

    std::cout << "# Encrypt training data ... " << std::flush;

    start = std::chrono::high_resolution_clock::now();

    auto encryptedTrainingData = ckksClient.Encrypt(trainingData);

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

    //-----------------------------------------------------------------------------------------------------------------

    // Step 05
    // Encrypt testing data

    std::cout << "# Encrypt testing data ... " << std::flush;

    start = std::chrono::high_resolution_clock::now();

    auto encryptedTestingData = ckksClient.Encrypt(testingData);

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

    // S E R V E R   S I D E   P R O C E S S I N G --------------------------------------------------------------------

    std::cout << std::endl;
    std::cout << ">>> SERVER SIDE PROCESSING" << std::endl;

    start = std::chrono::high_resolution_clock::now();
    auto clf = CkksKnnEncrypted(5, ckksCtx);

    std::cout << "# Train model " << std::endl;
    // review encrypted label
    // clf.Fit(encryptedTrainingData, trainingLabels);

    std::cout << "# Test model ... " << std::flush;
    clf.Predict(encryptedTestingData[0]);

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;
    */
    return 0;
}
