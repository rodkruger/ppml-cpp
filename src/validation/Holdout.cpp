#include <algorithm>
#include <iostream>
#include <ostream>
#include <unordered_map>
#include <vector>
#include <random>

#include "validation.h"

namespace hermesml {
    Holdout::Holdout(const std::vector<std::vector<double> > &features,
                     const std::vector<double> &labels) : features(features), labels(labels),
                                                          gen(std::random_device{}()) {
    }

    void Holdout::Split(const double trainingRatio) {
        // 1. Group features by labels
        std::unordered_map<double, std::vector<size_t> > groupedIndexes;
        for (size_t i = 0; i < labels.size(); i++) {
            groupedIndexes[labels[i]].push_back(i);
        }

        // 2. Shuffle each group and split based on trainingRatio
        for (auto &[key, indexes]: groupedIndexes) {
            // 3. Shuffle indexes
            std::shuffle(indexes.begin(), indexes.end(), gen);

            // 4. Determine split point
            const auto trainingDatasetSize = static_cast<size_t>(static_cast<double>(indexes.size()) * trainingRatio);

            // 5. Add to training and testing datasets
            for (size_t i = 0; i < indexes.size(); i++) {
                const size_t idx = indexes[i];
                if (i < trainingDatasetSize) {
                    trainingFeatures.push_back(features[idx]);
                    trainingLabels.push_back(labels[idx]);
                } else {
                    testingFeatures.push_back(features[idx]);
                    testingLabels.push_back(labels[idx]);
                }
            }
        }

        this->Shuffle(trainingFeatures, trainingLabels);
        this->Shuffle(testingFeatures, testingLabels);
    }

    void Holdout::Shuffle(std::vector<std::vector<double> > features, std::vector<double> labels) {
        std::vector<size_t> indices(labels.size());
        for (size_t i = 0; i < indices.size(); i++) {
            indices[i] = i;
        }

        std::shuffle(indices.begin(), indices.end(), gen);

        std::vector<std::vector<double> > shuffledFeatures(features.size());
        std::vector<double> shuffledLabels(labels.size());
        for (size_t i = 0; i < indices.size(); ++i) {
            shuffledFeatures[i] = features[indices[i]];
            shuffledLabels[i] = labels[indices[i]];
        }

        features = std::move(shuffledFeatures);
        labels = std::move(shuffledLabels);
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
