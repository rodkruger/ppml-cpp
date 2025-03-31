#include "datasets.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

namespace hermesml {
    Dataset::Dataset(const std::string &name, const DatasetRanges &range) {
        this->name = name;
        this->range = range;
        this->contentPath = std::filesystem::current_path().string() + "/" + this->name + "/";
        // this->contentPath = "/home/rkruger/Doutorado/Datasets/" + this->name + "/";
    }

    std::string Dataset::GetContentPath() const {
        return this->contentPath;
    }

    std::string Dataset::GetName() const {
        return this->name;
    }

    std::vector<std::vector<double> > Dataset::ReadFeatures(const std::string &fileName) const {
        const auto filePath = this->contentPath + fileName;
        std::vector<std::vector<double> > data;
        std::ifstream file(filePath);

        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath);
        }

        std::string line;
        while (std::getline(file, line)) {
            std::vector<double> row;
            std::stringstream ss(line);
            std::string value;

            while (std::getline(ss, value, ',')) {
                row.emplace_back(std::stod(value));
            }

            data.emplace_back(row);
        }

        file.close();

        return data;
    }

    std::vector<double> Dataset::ReadLabels(const std::string &fileName) const {
        const auto filePath = this->contentPath + fileName;
        std::vector<double> data;
        std::ifstream file(filePath);

        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + filePath);
        }

        std::string line;
        while (std::getline(file, line)) {
            std::vector<double> row;
            std::stringstream ss(line);
            std::string value;

            while (std::getline(ss, value, ',')) {
                auto dValue = std::stod(value);
                data.emplace_back(dValue);
                break;
            }
        }

        file.close();

        return data;
    }

    std::vector<std::vector<double> > Dataset::GetTrainingFeatures() {
        switch (this->range) {
            case FM22: return this->ReadFeatures("training_features_range22.csv");
            case F01: return this->ReadFeatures("training_features_range01.csv");
            case F11: return this->ReadFeatures("training_features_range11.csv");
            default: return {};
        }
    }

    std::vector<double> Dataset::GetTrainingLabels() {
        switch (this->range) {
            case FM22: return this->ReadLabels("training_labels_range22.csv");
            case F01: return this->ReadLabels("training_labels_range01.csv");
            case F11: return this->ReadLabels("training_labels_range11.csv");
            default: return {};
        }
    }

    std::vector<std::vector<double> > Dataset::GetTestingFeatures() {
        switch (this->range) {
            case FM22: return this->ReadFeatures("testing_features_range22.csv");
            case F01: return this->ReadFeatures("testing_features_range01.csv");
            case F11: return this->ReadFeatures("testing_features_range11.csv");
            default: return {};
        }
    }

    std::vector<double> Dataset::GetTestingLabels() {
        switch (this->range) {
            case FM22: return this->ReadLabels("testing_labels_range22.csv");
            case F01: return this->ReadLabels("testing_labels_range01.csv");
            case F11: return this->ReadLabels("testing_labels_range11.csv");
            default: return {};
        }
    }
}
