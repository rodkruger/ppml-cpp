#include "datasets.h"
#include "experiments.h"

using namespace hermesml;

int main() {
    constexpr auto epochs = 10;

    std::vector<std::unique_ptr<Dataset> > datasets11;
    datasets11.emplace_back(std::make_unique<BreastCancerDataset>(F11));
    datasets11.emplace_back(std::make_unique<DiabetesDataset>(F11));
    datasets11.emplace_back(std::make_unique<GliomaGradingDataset>(F11));
    datasets11.emplace_back(std::make_unique<DifferentiatedThyroidDataset>(F11));
    datasets11.emplace_back(std::make_unique<CirrhosisPatientDataset>(F11));
    // datasets11.emplace_back(std::make_unique<LoanPredictionDataset>(F11));
    // datasets11.emplace_back(std::make_unique<CreditCardFraudDataset>(F11));

    std::vector<std::unique_ptr<Dataset> > datasets01;
    datasets01.emplace_back(std::make_unique<BreastCancerDataset>(F01));
    datasets01.emplace_back(std::make_unique<DiabetesDataset>(F01));
    datasets01.emplace_back(std::make_unique<GliomaGradingDataset>(F01));
    datasets01.emplace_back(std::make_unique<DifferentiatedThyroidDataset>(F01));
    datasets01.emplace_back(std::make_unique<CirrhosisPatientDataset>(F01));
    // datasets01.emplace_back(std::make_unique<LoanPredictionDataset>(F01));
    // datasets01.emplace_back(std::make_unique<CreditCardFraudDataset>(F01));

    CkksLogisticRegressionExperimentParams params{};

    for (auto i = 1; i <= epochs; i++) {
        for (auto j = 0; j < datasets11.size(); j++) {
            params.activation = CkksLogisticRegression::TANH;
            params.epochs = i;
            params.earlyBootstrapping = 0;
            CkksLogisticRegressionExperiment("ckks_tanh_" + std::to_string(params.epochs), *datasets11[j], params).Run();

            params.activation = CkksLogisticRegression::SIGMOID;
            params.epochs = i;
            params.earlyBootstrapping = 0;
            CkksLogisticRegressionExperiment("ckks_sigmoid_" + std::to_string(params.epochs), *datasets01[j], params).Run();
        }
    }
}
