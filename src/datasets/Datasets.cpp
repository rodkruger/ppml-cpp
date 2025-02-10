#include "datasets.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

namespace hermesml {
    Dataset::Dataset(const std::string &name, const std::string &filePath) {
        if (!std::filesystem::exists(filePath)) {
            throw std::runtime_error("File not found: " + filePath);
        }

        this->name = name;
        this->filePath = filePath;
    }

    std::string Dataset::GetName() const {
        return this->name;
    }

    std::vector<std::vector<double> > Dataset::ReadFeatures(const std::string &fileName) const {
        std::vector<std::vector<double> > data;
        std::ifstream file("/home/rkruger/Doutorado/Datasets/" + this->name + "/" + fileName);

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

    std::vector<double> Dataset::ReadLabels(const std::string &fileName) const {
        std::vector<double> data;
        std::ifstream file("/home/rkruger/Doutorado/Datasets/" + this->name + "/" + fileName);

        if (!file.is_open()) {
            throw std::runtime_error("Could not open file: " + this->filePath);
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

    std::vector<std::vector<double> > Dataset::GetFeatures() const {
        return this->features;
    }

    std::vector<double> Dataset::GetLabels() const {
        return this->labels;
    }

    std::vector<std::string> Dataset::Split(const std::string &line, const char delimiter) {
        std::vector<std::string> tokens;
        std::stringstream ss(line);
        std::string token;
        while (std::getline(ss, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }

    void Dataset::Read() {
        std::ifstream file(this->filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Unable to open file: " + this->filePath);
        }

        std::string line;
        bool isHeader = true;
        while (std::getline(file, line)) {
            if (isHeader) {
                // Skip the header line
                isHeader = false;
                continue;
            }

            std::vector<std::string> tokens = Split(line, ',');
            std::vector<double> featureRow;

            // Convert features to double and add to feature vector
            for (size_t i = 0; i < tokens.size() - 1; i++) {
                featureRow.push_back(std::stod(tokens[i]));
            }

            // Add the features row
            this->features.push_back(featureRow);

            // Add the label as a double value as well
            this->labels.push_back(std::stod(tokens.back()));
        }

        file.close();
    }
}
