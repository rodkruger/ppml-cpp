#include "experiments.h"

int main() {
    for (uint16_t epoch = 1; epoch <= 10; epoch++) {
        const auto epoch_str = std::to_string(epoch);
        hermesml::CkksPerceptronExperiment("ckks_tanh_" + epoch_str, hermesml::CkksPerceptron::TANH, epoch).Run();
        hermesml::CkksPerceptronExperiment("ckks_sigmoid_" + epoch_str, hermesml::CkksPerceptron::SIGMOID, epoch).Run();
    }

    return EXIT_SUCCESS;
}
