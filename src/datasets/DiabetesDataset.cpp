#include <fstream>

#include "datasets.h"

namespace hermesml {
    DiabetesDataset::DiabetesDataset(const DatasetRanges range) : Dataset("diabetes",
                                                                          "/home/rkruger/Doutorado/Datasets/diabetes/diabetes.csv"),
                                                                  range(range) {
    }

    std::vector<std::string> DiabetesDataset::Split(const std::string &line, const char delimiter) {
        return {};
    }

    std::vector<std::vector<double> > DiabetesDataset::GetTrainingFeatures() {
        switch (this->range) {
            case FM22: return this->ReadFeatures("training_features_range22.csv");
            case F01: return this->ReadFeatures("training_features_range01.csv");
            case F11: return this->ReadFeatures("training_features_range11.csv");
            default: return {};
        }
    }

    std::vector<double> DiabetesDataset::GetTrainingLabels() {
        switch (this->range) {
            case FM22: return this->ReadLabels("training_labels_range22.csv");
            case F01: return this->ReadLabels("training_labels_range01.csv");
            case F11: return this->ReadLabels("training_labels_range11.csv");
            default: return {};
        }
    }

    std::vector<std::vector<double> > DiabetesDataset::GetTestingFeatures() {
        switch (this->range) {
            case FM22: return this->ReadFeatures("testing_features_range22.csv");
            case F01: return this->ReadFeatures("testing_features_range01.csv");
            case F11: return this->ReadFeatures("testing_features_range11.csv");
            default: return {};
        }
    }

    std::vector<double> DiabetesDataset::GetTestingLabels() {
        switch (this->range) {
            case FM22: return this->ReadLabels("testing_labels_range22.csv");
            case F01: return this->ReadLabels("testing_labels_range01.csv");
            case F11: return this->ReadLabels("testing_labels_range11.csv");
            default: return {};
        }
    }
}
