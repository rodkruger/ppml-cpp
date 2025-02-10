#include "datasets.h"
#include "experiments.h"

using namespace hermesml;

int main() {
    CkksPerceptronExperimentParams params{};

    constexpr auto epoch_begin = 1;
    constexpr auto epoch_end = 10;

    auto dataset = DiabetesDataset(F11);

    for (uint16_t epoch = epoch_begin; epoch <= epoch_end; epoch++) {
        const auto epoch_str = std::to_string(epoch);

        params.activation = hermesml::CkksPerceptron::TANH;
        params.epochs = epoch;
        params.earlyBootstrapping = 0;
        CkksPerceptronExperiment("ckks_tanh_" + epoch_str, dataset, params).Run();

        params.activation = hermesml::CkksPerceptron::SIGMOID;
        params.epochs = epoch;
        params.earlyBootstrapping = 0;
        CkksPerceptronExperiment("ckks_sigmoid_" + epoch_str, dataset, params).Run();

        /*
        params.activation = hermesml::CkksPerceptron::IDENTITY;
        params.epochs = epoch;
        params.earlyBootstrapping = 0;
        hermesml::CkksPerceptronExperiment("ckks_identity_" + epoch_str, dataset, params).Run();
        */
    }

    return EXIT_SUCCESS;
}
