#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include "core.h"
#include "model.h"

namespace hermesml {
    struct CkksLogisticRegressionExperimentParams {
        CkksLogisticRegression::Activation activation;
        uint16_t epochs;
        int8_t earlyBootstrapping;
        int8_t scalingAlpha;
        int8_t scalingBeta;
    };

    class CkksLogisticRegressionExperiment : public Experiment {
        CkksLogisticRegressionExperimentParams params;

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
        explicit CkksLogisticRegressionExperiment(const std::string &experimentId,
                                          Dataset &dataset,
                                          const CkksLogisticRegressionExperimentParams &params);

        void Run() override;

        void RunMemory();

        void RunHardDisk();
    };
}

#endif //EXPERIMENTS_H
