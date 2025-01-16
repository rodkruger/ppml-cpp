#include "datasets.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

namespace hermesml {
    Dataset::Dataset(const std::string &filePath) {
        if (!std::filesystem::exists(filePath)) {
            throw std::runtime_error("File not found: " + filePath);
        }

        this->filePath = filePath;
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
