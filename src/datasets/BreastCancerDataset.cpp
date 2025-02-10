#include <fstream>

#include "datasets.h"

namespace hermesml {
    BreastCancerDataset::BreastCancerDataset(const DatasetRanges range) : Dataset("breast_cancer",
                                                                              "/home/rkruger/Doutorado/Datasets/breast_cancer/data.csv"),
                                                                          range(range) {
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

    std::vector<std::vector<double> > BreastCancerDataset::GetTrainingFeatures() {
        switch (this->range) {
            case FM22: return this->ReadFeatures("training_features_range22.csv");
            case F01: return this->ReadFeatures("training_features_range01.csv");
            case F11: return this->ReadFeatures("training_features_range11.csv");
            default: return {};
        }
    }

    std::vector<double> BreastCancerDataset::GetTrainingLabels() {
        switch (this->range) {
            case FM22: return this->ReadLabels("training_labels_range22.csv");
            case F01: return this->ReadLabels("training_labels_range01.csv");
            case F11: return this->ReadLabels("training_labels_range11.csv");
            default: return {};
        }
    }

    std::vector<std::vector<double> > BreastCancerDataset::GetTestingFeatures() {
        switch (this->range) {
            case FM22: return this->ReadFeatures("testing_features_range22.csv");
            case F01: return this->ReadFeatures("testing_features_range01.csv");
            case F11: return this->ReadFeatures("testing_features_range11.csv");
            default: return {};
        }
    }

    std::vector<double> BreastCancerDataset::GetTestingLabels() {
        switch (this->range) {
            case FM22: return this->ReadLabels("testing_labels_range22.csv");
            case F01: return this->ReadLabels("testing_labels_range01.csv");
            case F11: return this->ReadLabels("testing_labels_range11.csv");
            default: return {};
        }
    }
}
