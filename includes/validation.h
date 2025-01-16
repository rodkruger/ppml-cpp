#ifndef VALIDATION_H
#define VALIDATION_H

#include <vector>

namespace hermesml {
    class Holdout {
        std::vector<std::vector<double> > features;
        std::vector<double> labels;
        std::vector<std::vector<double> > trainingFeatures;
        std::vector<std::vector<double> > testingFeatures;
        std::vector<double> trainingLabels;
        std::vector<double> testingLabels;
        std::mt19937 gen;

        void Shuffle(std::vector<std::vector<double> > features, std::vector<double> labels);

    public:
        Holdout(const std::vector<std::vector<double> > &features, const std::vector<double> &labels);

        void Split(double trainingRatio);

        [[nodiscard]] std::vector<std::vector<double> > GetFeatures() const;

        [[nodiscard]] std::vector<double> GetLabels() const;

        [[nodiscard]] std::vector<std::vector<double> > GetTrainingFeatures() const;

        [[nodiscard]] std::vector<double> GetTrainingLabels() const;

        [[nodiscard]] std::vector<std::vector<double> > GetTestingFeatures() const;

        [[nodiscard]] std::vector<double> GetTestingLabels() const;
    };
}

#endif //VALIDATION_H
