#include <iostream>
#include "csv.hpp"
#include "model.h"
#include "client.h"

using namespace hermesml;

//---------------------------------------------------------------------------------------------------------------------

void read_wine_dataset(std::vector<std::vector<double>>& features, std::vector<double>& labels)
{
    const std::string filename = "../datasets/wine/wine.data";

    // Create a CSV reader object with specified column names
    csv::CSVFormat format;
    format.column_names({
        "Class",
        "Alcohol",
        "Malic acid",
        "Ash",
        "Alcalinity of ash",
        "Magnesium",
        "Total phenols",
        "Flavanoids",
        "Nonflavanoid phenols",
        "Proanthocyanins",
        "Color intensity",
        "Hue",
        "OD280/OD315 of diluted wines",
        "Proline"
    });

    // Initialize the CSV reader
    csv::CSVReader reader(filename, format);

    // Iterate over each row in the CSV file
    for (csv::CSVRow& row : reader)
    {
        // Extract the class label and append to labels vector
        int wine_class = row["Class"].get<int>();
        labels.push_back(wine_class);

        // Extract features and append to features vector
        std::vector<double> sample_features;
        for (size_t i = 1; i < row.size(); ++i)
        {
            auto value = row[i].get<double>();
            sample_features.push_back(value);
        }
        features.push_back(sample_features);
    }
}

//---------------------------------------------------------------------------------------------------------------------

std::vector<std::vector<int64_t>> quantize_features(std::vector<std::vector<double>>& features)
{
    // MinMaxScaler().Scale(features);
    return Quantizer().Quantize(features);
}

std::vector<std::vector<int64_t>> quantize_labels(std::vector<double>& labels, int32_t n_features)
{
    auto transf_labels = std::vector<std::vector<double>>();

    for (const auto label : labels)
    {
        transf_labels.emplace_back(n_features, label);
    }

    return Quantizer().Quantize(transf_labels);
}

//---------------------------------------------------------------------------------------------------------------------

void split_train_and_test(std::vector<std::vector<int64_t>>& quant_features,
                          std::vector<std::vector<int64_t>>& quant_labels,
                          std::vector<std::vector<int64_t>>& training_data,
                          std::vector<std::vector<int64_t>>& testing_data,
                          std::vector<std::vector<int64_t>>& training_labels,
                          std::vector<std::vector<int64_t>>& testing_labels)
{
    const auto dataset_length = static_cast<int64_t>(quant_features.size());
    const auto training_length = static_cast<int64_t>(std::round(dataset_length) * 0.7);
    const auto testing_length = dataset_length - training_length;

    training_data.reserve(training_length);
    training_labels.reserve(testing_length);
    testing_data.reserve(training_length);
    testing_labels.reserve(testing_length);

    std::copy(quant_features.begin(), quant_features.begin() + training_length, std::back_inserter(training_data));
    std::copy(quant_labels.begin(), quant_labels.begin() + training_length, std::back_inserter(training_labels));
    std::copy(quant_features.begin() + training_length, quant_features.end(), std::back_inserter(testing_data));
    std::copy(quant_labels.begin() + training_length, quant_labels.end(), std::back_inserter(testing_labels));
}

//---------------------------------------------------------------------------------------------------------------------

int main()
{
    std::vector<std::vector<double>> features;
    std::vector<double> labels;
    std::chrono::time_point<std::chrono::system_clock> start, end;
    std::chrono::duration<double> elapsed;

    std::cout << ">>> CLIENT SIDE PROCESSING" << std::endl;

    // Step 01 - read and normalize data
    std::cout << "# Read dataset " << std::endl;
    read_wine_dataset(features, labels);
    auto quant_features = quantize_features(features);
    auto quant_labels = quantize_labels(labels, features.size());

    // Step 02 - split dataset in training and testing. Holdout (70% training; 30% testing)
    std::cout << "# Split dataset (70% training; 30% testing)" << std::endl;

    auto training_data = std::vector<std::vector<int64_t>>();
    auto testing_data = std::vector<std::vector<int64_t>>();
    auto training_labels = std::vector<std::vector<int64_t>>();
    auto testing_labels = std::vector<std::vector<int64_t>>();

    split_train_and_test(quant_features, quant_labels, training_data, testing_data, training_labels, testing_labels);

    std::cout << "      Total samples: " << (training_data.size() + testing_data.size()) << std::endl;
    std::cout << "      Training length: " << training_data.size() << std::endl;
    std::cout << "      Testing length: " << testing_data.size() << std::endl;

    //-----------------------------------------------------------------------------------------------------------------

    // Step 03
    // Generating keys and crypto context. Do not share it with anybody :)

    std::cout << "# Generate crypto context" << std::endl;

    auto bgvCtx = HEContextFactory().bgvHeContext();
    auto bgvClient = Client(bgvCtx);
    auto cc = bgvCtx.GetCc();

    std::cout << "      Ring dimension: " << cc->GetRingDimension() << std::endl;
    std::cout << "      Modulus: " << cc->GetModulus() << std::endl;
    std::cout << "      Multiplicative depth: " << bgvCtx.GetMultiplicativeDepth() << std::endl;

    //-----------------------------------------------------------------------------------------------------------------

    // Step 04
    // Encrypt training data

    std::cout << "# Encrypt training data ... " << std::flush;

    start = std::chrono::high_resolution_clock::now();

    auto encrypted_training_data = bgvClient.Encrypt(training_data);
    auto encrypted_training_labels = bgvClient.Encrypt(training_labels);

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

    //-----------------------------------------------------------------------------------------------------------------

    // Step 05
    // Encrypt testing data

    std::cout << "# Encrypt testing data ... " << std::flush;

    start = std::chrono::high_resolution_clock::now();

    auto encrypted_testing_data = bgvClient.Encrypt(testing_data);
    auto encrypted_testing_labels = bgvClient.Encrypt(testing_labels);

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

    // S E R V E R   S I D E   P R O C E S S I N G --------------------------------------------------------------------

    std::cout << std::endl;
    std::cout << ">>> SERVER SIDE PROCESSING" << std::endl;

    auto clf = LogisticRegressionEncrypted(bgvCtx, features[0].size(), 3);

    std::cout << "# Train model ... " << std::flush;

    start = std::chrono::high_resolution_clock::now();

    clf.Fit(encrypted_training_data, encrypted_training_labels);

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

    return EXIT_SUCCESS;

    std::cout << "# Test model ... " << std::flush;

    start = std::chrono::high_resolution_clock::now();

    auto encrypted_label = clf.Predict(encrypted_testing_data[0]);

    end = std::chrono::high_resolution_clock::now();
    elapsed = end - start;

    std::cout << "Elapsed time: " << elapsed.count() << " ms" << std::endl;

    Plaintext plain_label;
    cc->Decrypt(bgvCtx.GetPrivateKey(), encrypted_label, &plain_label);
    std::cout << "# Decrypted label: " << plain_label << std::endl;

    // 420.000 -> 17 hours / 1020 minutes
    // 200 -> x = 0,48 (appr. 30 seconds)

    // Multiplicative Depth: 20 (minimum value)
    // Encryption Time: 60 seconds
    // Training Time: 950 seconds
}
