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
        DatasetRanges range;
        std::vector<std::vector<double> > features;
        std::vector<double> labels;
        std::string contentPath;

        [[nodiscard]] std::vector<std::vector<double> > ReadFeatures(const std::string &fileName) const;

        [[nodiscard]] std::vector<double> ReadLabels(const std::string &fileName) const;

    public:
        virtual ~Dataset() = default;

        explicit Dataset(const std::string &name, const DatasetRanges &range);

        [[nodiscard]] std::string GetName() const;

        [[nodiscard]] virtual std::vector<std::vector<double> > GetTrainingFeatures();

        [[nodiscard]] virtual std::vector<double> GetTrainingLabels();

        [[nodiscard]] virtual std::vector<std::vector<double> > GetTestingFeatures();

        [[nodiscard]] virtual std::vector<double> GetTestingLabels();
    };

    class BreastCancerDataset final : public Dataset {
    public:
        explicit BreastCancerDataset(DatasetRanges range);
    };

    class DiabetesDataset final : public Dataset {
    public:
        explicit DiabetesDataset(DatasetRanges range);
    };

    class GliomaGradingDataset final : public Dataset {
    public:
        explicit GliomaGradingDataset(DatasetRanges range);
    };

    class DifferentiatedThyroidDataset final : public Dataset {
    public:
        explicit DifferentiatedThyroidDataset(DatasetRanges range);
    };

    class CirrhosisPatientDataset final : public Dataset {
    public:
        explicit CirrhosisPatientDataset(DatasetRanges range);
    };

    class LoanPredictionDataset final : public Dataset {
    public:
        explicit LoanPredictionDataset(DatasetRanges range);
    };

    class CreditCardFraudDataset final : public Dataset {
    public:
        explicit CreditCardFraudDataset(DatasetRanges range);
    };

}


#endif //DATASETS_H
