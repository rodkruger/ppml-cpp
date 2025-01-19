#include "experiments.h"

int main() {
    for (uint16_t epoch = 1; epoch <= 20; epoch++) {
        auto experiment_name = "exp_" + std::to_string(epoch);
        hermesml::CkksPerceptronExperiment(experiment_name, epoch).Run();
    }

    return EXIT_SUCCESS;
}
