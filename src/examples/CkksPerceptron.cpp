#include "experiments.h"

int main() {
    auto e1 = hermesml::CkksPerceptronExperiment("exp_001");

    e1.Run();

    return EXIT_SUCCESS;
}
