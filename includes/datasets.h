//
// Created by rkruger on 09/01/25.
//

#ifndef DATASETS_H
#define DATASETS_H

#include <string>
#include <vector>
#include <filesystem>

namespace hermesml {
    enum DatasetRanges {
        FM22, F11, F01
    };

    class Dataset {
    protected:
        std::string name;
        std::vector<std::vector<double> > features;
        std::vector<double> labels;
        std::string filePath;

        [[nodiscard]] std::vector<std::vector<double> > ReadFeatures(const std::string &fileName) const;

        [[nodiscard]] std::vector<double> ReadLabels(const std::string &fileName) const;

    public:
        virtual ~Dataset() = default;

        explicit Dataset(const std::string &name, const std::string &filePath);

        [[nodiscard]] std::string GetName() const;

        [[nodiscard]] std::vector<std::vector<double> > GetFeatures() const;

        [[nodiscard]] std::vector<double> GetLabels() const;

        void Read();

        [[nodiscard]] virtual std::vector<std::string> Split(const std::string &line, char delimiter);

        [[nodiscard]] virtual std::vector<std::vector<double> > GetTrainingFeatures() = 0;

        [[nodiscard]] virtual std::vector<double> GetTrainingLabels() = 0;

        [[nodiscard]] virtual std::vector<std::vector<double> > GetTestingFeatures() = 0;

        [[nodiscard]] virtual std::vector<double> GetTestingLabels() = 0;
    };

    class BreastCancerDataset : public Dataset {
        DatasetRanges range;

    public:
        explicit BreastCancerDataset(DatasetRanges range);

        [[nodiscard]] std::vector<std::string> Split(const std::string &line, char delimiter) override;

        [[nodiscard]] std::vector<std::vector<double> > GetTrainingFeatures() override;

        [[nodiscard]] std::vector<double> GetTrainingLabels() override;

        [[nodiscard]] std::vector<std::vector<double> > GetTestingFeatures() override;

        [[nodiscard]] std::vector<double> GetTestingLabels() override;
    };

    class DiabetesDataset : public Dataset {
        DatasetRanges range;

    public:
        explicit DiabetesDataset(DatasetRanges range);

        [[nodiscard]] std::vector<std::string> Split(const std::string &line, char delimiter) override;

        [[nodiscard]] std::vector<std::vector<double> > GetTrainingFeatures() override;

        [[nodiscard]] std::vector<double> GetTrainingLabels() override;

        [[nodiscard]] std::vector<std::vector<double> > GetTestingFeatures() override;

        [[nodiscard]] std::vector<double> GetTestingLabels() override;
    };
}


#endif //DATASETS_H
