#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include "core.h"
#include "model.h"

namespace hermesml {
    class CkksPerceptronExperiment : public Experiment {
    public:
        explicit CkksPerceptronExperiment(const std::string &experimentId, CkksPerceptron::Activation activation,
                                          uint16_t epochs);

        void Run() override;

    private:
        CkksPerceptron::Activation activation;
        uint16_t epochs;
        size_t datasetLength{};
        double trainingRatio{0.7};
        size_t trainingLength{};
        size_t testingLength{};
        size_t ringDimension{};
        size_t multiplicativeDepth{};

        std::chrono::duration<double> encryptingTime{};
        std::chrono::duration<double> trainingTime{};
        std::chrono::duration<double> testingTime{};

    public:
    };
}

#endif //EXPERIMENTS_H
