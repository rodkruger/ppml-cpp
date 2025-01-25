#include "experiments.h"

int main() {
    for (uint16_t epoch = 1; epoch <= 10; epoch++) {
        const auto epoch_str = std::to_string(epoch);
        // hermesml::CkksPerceptronExperiment("ckks_tanh_" + epoch_str, hermesml::CkksPerceptron::TANH, epoch, 0).Run();
        hermesml::CkksPerceptronExperiment("ckks_sigmoid_" + epoch_str, hermesml::CkksPerceptron::SIGMOID, epoch, 1).
                Run();
    }

    return EXIT_SUCCESS;
}
