#include <algorithm>
#include <cmath>
#include <cstdint>
#include <vector>

#include "validation.h"

namespace hermesml {
    Holdout::Holdout(const std::vector<std::vector<double> > &features,
                     const std::vector<double> &labels) : features(features), labels(labels) {
    }

    void Holdout::Split() {
        const auto datasetLength = static_cast<int64_t>(features.size());
        const auto trainingLength = static_cast<int64_t>(std::round(datasetLength) * 0.7);
        const auto testingLength = datasetLength - trainingLength;

        this->trainingFeatures.reserve(trainingLength);
        this->trainingLabels.reserve(trainingLength);
        this->testingFeatures.reserve(testingLength);
        this->testingLabels.reserve(testingLength);

        std::copy_n(features.begin(), trainingLength, std::back_inserter(this->trainingFeatures));
        std::copy_n(labels.begin(), trainingLength, std::back_inserter(this->trainingLabels));
        std::copy(features.begin() + trainingLength, features.end(), std::back_inserter(this->testingFeatures));
        std::copy(labels.begin() + trainingLength, labels.end(), std::back_inserter(this->testingLabels));
    }

    std::vector<std::vector<double> > Holdout::GetFeatures() const {
        return this->features;
    }

    std::vector<double> Holdout::GetLabels() const {
        return this->labels;
    }

    std::vector<std::vector<double> > Holdout::GetTrainingFeatures() const {
        return this->trainingFeatures;
    }

    std::vector<double> Holdout::GetTrainingLabels() const {
        return this->trainingLabels;
    }

    std::vector<std::vector<double> > Holdout::GetTestingFeatures() const {
        return this->testingFeatures;
    }

    std::vector<double> Holdout::GetTestingLabels() const {
        return this->testingLabels;
    }
}
