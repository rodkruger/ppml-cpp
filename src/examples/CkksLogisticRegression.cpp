#include "datasets.h"
#include "experiments.h"

using namespace hermesml;

int main() {
    constexpr auto epochs = 10;

    std::vector<std::unique_ptr<Dataset> > datasets11;
    datasets11.emplace_back(std::make_unique<BreastCancerDataset>(FM11));
    datasets11.emplace_back(std::make_unique<DiabetesDataset>(FM11));
    datasets11.emplace_back(std::make_unique<GliomaGradingDataset>(FM11));
    datasets11.emplace_back(std::make_unique<DifferentiatedThyroidDataset>(FM11));
    datasets11.emplace_back(std::make_unique<CirrhosisPatientDataset>(FM11));

    CkksLogisticRegressionExperimentParams params{};

    for (auto i = 1; i <= epochs; i++) {
        for (auto j = 0; j < datasets11.size(); j++) {
            params.activation = TANH;
            params.approximation = CHEBYSHEV;
            params.epochs = i;
            params.earlyBootstrapping = 0;
            CkksLogisticRegressionExperiment("ckks_tanh_chebyshev_" + std::to_string(params.epochs), *datasets11[j],
                                             params).Run();

            params.activation = TANH;
            params.approximation = TAYLOR;
            params.epochs = i;
            params.earlyBootstrapping = 0;
            CkksLogisticRegressionExperiment("ckks_tanh_taylor_" + std::to_string(params.epochs), *datasets11[j],
                                             params).Run();

            params.activation = TANH;
            params.approximation = LEAST_SQUARES;
            params.epochs = i;
            params.earlyBootstrapping = 0;
            CkksLogisticRegressionExperiment("ckks_tanh_least_squares_" + std::to_string(params.epochs), *datasets11[j],
                                             params).Run();

            params.activation = SIGMOID;
            params.approximation = CHEBYSHEV;
            params.epochs = i;
            params.earlyBootstrapping = 0;
            CkksLogisticRegressionExperiment("ckks_sigmoid_chebyshev_" + std::to_string(params.epochs), *datasets11[j],
                                             params).Run();

            params.activation = SIGMOID;
            params.approximation = TAYLOR;
            params.epochs = i;
            params.earlyBootstrapping = 0;
            CkksLogisticRegressionExperiment("ckks_sigmoid_taylor_" + std::to_string(params.epochs), *datasets11[j],
                                             params).Run();

            params.activation = SIGMOID;
            params.approximation = LEAST_SQUARES;
            params.epochs = i;
            params.earlyBootstrapping = 0;
            CkksLogisticRegressionExperiment("ckks_sigmoid_least_squares_" + std::to_string(params.epochs),
                                             *datasets11[j],
                                             params).Run();
        }
    }
}
