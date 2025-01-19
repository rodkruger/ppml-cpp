#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include "core.h"

namespace hermesml {
    class CkksPerceptronExperiment : public Experiment {
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
        explicit CkksPerceptronExperiment(const std::string &experimentId, uint16_t epochs);

        void Run() override;
    };
}

#endif //EXPERIMENTS_H
