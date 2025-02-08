#include <fstream>

#include "datasets.h"

namespace hermesml {
    BreastCancerDataset::BreastCancerDataset() : Dataset("/home/rkruger/Doutorado/Datasets/breast_cancer/data.csv") {
        this->Read();
    }

    std::vector<std::string> BreastCancerDataset::Split(const std::string &line, const char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        std::string label;

        int columnIndex = 0;
        while (std::getline(ss, token, delimiter)) {
            if (columnIndex == 1) {
                if (token == "B") {
                    label = "0.0";
                } else {
                    label = "1.0";
                }
            } else {
                if (columnIndex >= 2) {
                    tokens.push_back(token);
                }
            }

            columnIndex++;
        }

        tokens.push_back(label);
        return tokens;
    }

    std::vector<std::vector<double> > BreastCancerDataset::ReadFeatures(const std::string &fileName) {
        std::vector<std::vector<double> > data;
        std::ifstream file("/home/rkruger/Doutorado/Datasets/breast_cancer/" + fileName);

        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + fileName);
        }

        std::string line;
        while (std::getline(file, line)) {
            std::vector<double> row;
            std::stringstream ss(line);
            std::string value;

            while (std::getline(ss, value, ',')) {
                row.push_back(std::stod(value));
            }

            data.push_back(row);
        }

        file.close();

        return data;
    }

    std::vector<double> BreastCancerDataset::ReadLabels(const std::string &fileName) {
        std::vector<double> data;
        std::ifstream file("/home/rkruger/Doutorado/Datasets/breast_cancer/" + fileName);

        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + fileName);
        }

        std::string line;
        while (std::getline(file, line)) {
            std::vector<double> row;
            std::stringstream ss(line);
            std::string value;

            while (std::getline(ss, value, ',')) {
                auto dValue = std::stod(value);

                if (dValue == 0.0) {
                    dValue = -1.0;
                }

                data.push_back(dValue);
                break;
            }
        }

        file.close();

        return data;
    }

    std::vector<std::vector<double> > BreastCancerDataset::GetTrainingFeatures(const BreastCancerDatasetRanges range) {
        switch (range) {
            case FM22: return ReadFeatures("training_features_range22.csv");
            case F01: return ReadFeatures("training_features_range01.csv");
            case F11: return ReadFeatures("training_features_range11.csv");
            default: return {};
        }
    }

    std::vector<double> BreastCancerDataset::GetTrainingLabels(const BreastCancerDatasetRanges range) {
        switch (range) {
            case FM22: return ReadLabels("training_labels_range22.csv");
            case F01: return ReadLabels("training_labels_range01.csv");
            case F11: return ReadLabels("training_labels_range11.csv");
            default: return {};
        }
    }

    std::vector<std::vector<double> > BreastCancerDataset::GetTestingFeatures(const BreastCancerDatasetRanges range) {
        switch (range) {
            case FM22: return ReadFeatures("testing_features_range22.csv");
            case F01: return ReadFeatures("testing_features_range01.csv");
            case F11: return ReadFeatures("testing_features_range11.csv");
            default: return {};
        }
    }

    std::vector<double> BreastCancerDataset::GetTestingLabels(const BreastCancerDatasetRanges range) {
        switch (range) {
            case FM22: return ReadLabels("testing_labels_range22.csv");
            case F01: return ReadLabels("testing_labels_range01.csv");
            case F11: return ReadLabels("testing_labels_range11.csv");
            default: return {};
        }
    }
}
