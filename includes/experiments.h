#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include "core.h"
#include "model.h"

namespace hermesml {
    struct CkksPerceptronExperimentParams {
        CkksPerceptron::Activation activation;
        uint16_t epochs;
        int8_t earlyBootstrapping;
        int8_t scalingAlpha;
        int8_t scalingBeta;
    };

    class CkksPerceptronExperiment : public Experiment {
        CkksPerceptronExperimentParams params;

        size_t datasetLength{};
        double trainingRatio{0.7};
        size_t trainingLength{};
        size_t testingLength{};
        size_t ringDimension{};
        int8_t multiplicativeDepth{};

        std::chrono::duration<double> encryptingTime{};
        std::chrono::duration<double> trainingTime{};
        std::chrono::duration<double> testingTime{};

    public:
        explicit CkksPerceptronExperiment(const std::string &experimentId,
                                          const CkksPerceptronExperimentParams &params);

        void Run() override;
    };
}

#endif //EXPERIMENTS_H
