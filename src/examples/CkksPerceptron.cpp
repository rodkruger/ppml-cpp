#include "datasets.h"
#include "experiments.h"

using namespace hermesml;

int main() {
    constexpr auto epochs = 10;
    const auto epochs_str = std::to_string(epochs);

    std::vector<std::unique_ptr<Dataset> > datasets;
    datasets.emplace_back(std::make_unique<BreastCancerDataset>(F11));
    datasets.emplace_back(std::make_unique<DiabetesDataset>(F11));
    datasets.emplace_back(std::make_unique<GliomaGradingDataset>(F11));
    datasets.emplace_back(std::make_unique<DifferentiatedThyroidDataset>(F11));
    datasets.emplace_back(std::make_unique<CirrhosisPatientDataset>(F11));
    // datasets.emplace_back(std::make_unique<LoanPredictionDataset>(F11));
    // datasets.emplace_back(std::make_unique<CreditCardFraudDataset>(F11));

    CkksPerceptronExperimentParams params{};
    params.activation = CkksPerceptron::SIGMOID;
    params.epochs = epochs;
    params.earlyBootstrapping = 0;

    for (auto &dataset: datasets) {
        CkksPerceptronExperiment("ckks_sigmoid_" + dataset->GetName(), *dataset, params).Run();
    }

    params.activation = CkksPerceptron::TANH;
    params.epochs = epochs;
    params.earlyBootstrapping = 0;

    for (auto &dataset: datasets) {
        CkksPerceptronExperiment("ckks_tanh_" + dataset->GetName(), *dataset, params).Run();
    }
}
