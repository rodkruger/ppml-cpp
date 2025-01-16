//
// Created by rkruger on 09/01/25.
//

#ifndef DATASETS_H
#define DATASETS_H

#include <string>
#include <vector>
#include <filesystem>

namespace hermesml {
    class Dataset {
    protected:
        std::vector<std::vector<double> > features;
        std::vector<double> labels;
        std::string filePath;

    public:
        virtual ~Dataset() = default;

        explicit Dataset(const std::string &filePath);

        [[nodiscard]] std::vector<std::vector<double> > GetFeatures() const;

        [[nodiscard]] std::vector<double> GetLabels() const;

        void Read();

        [[nodiscard]] virtual std::vector<std::string> Split(const std::string &line, char delimiter);
    };

    class BreastCancerDataset : public Dataset {
    public:
        explicit BreastCancerDataset();

        [[nodiscard]] std::vector<std::string> Split(const std::string &line, char delimiter) override;
    };
}


#endif //DATASETS_H
