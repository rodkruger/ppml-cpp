#include "experiments.h"

int main() {
    hermesml::CkksPerceptronExperimentParams params{};

    for (uint16_t epoch = 1; epoch <= 10; epoch++) {
        const auto epoch_str = std::to_string(epoch);

        params.activation = hermesml::CkksPerceptron::SIGMOID;
        params.epochs = epoch;
        params.earlyBootstrapping = 9;
        params.scalingAlpha = -2;
        params.scalingBeta = 2;
        hermesml::CkksPerceptronExperiment("ckks_sigmoid_" + epoch_str, params).Run();
    }

    for (uint16_t epoch = 1; epoch <= 10; epoch++) {
        const auto epoch_str = std::to_string(epoch);

        params.activation = hermesml::CkksPerceptron::TANH;
        params.epochs = epoch;
        params.earlyBootstrapping = 0;
        params.scalingAlpha = 0;
        params.scalingBeta = 1;
        hermesml::CkksPerceptronExperiment("ckks_tanh_" + epoch_str, params).Run();
    }

    return EXIT_SUCCESS;
}
