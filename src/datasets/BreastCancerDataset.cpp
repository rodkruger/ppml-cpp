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
}
