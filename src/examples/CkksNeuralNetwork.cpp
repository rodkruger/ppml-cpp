#include "datasets.h"
#include "experiments.h"

using namespace hermesml;

int main(const int argc, char *argv[]) {
    constexpr auto epochs = 10;

    std::vector<std::unique_ptr<Dataset> > datasets11;

    std::map<std::string, std::function<std::unique_ptr<Dataset>()> > dataset_map = {
        {"diabetes", [&]() { return std::make_unique<DiabetesDataset>(FM11); }},
        {"breast", [&]() { return std::make_unique<BreastCancerDataset>(FM11); }},
        {"glioma", [&]() { return std::make_unique<GliomaGradingDataset>(FM11); }},
        {"thyroid", [&]() { return std::make_unique<DifferentiatedThyroidDataset>(FM11); }},
        {"cirrhosis", [&]() { return std::make_unique<CirrhosisPatientDataset>(FM11); }}
    };

    for (int i = 1; i < argc; ++i) {
        std::string name = argv[i];
        if (auto it = dataset_map.find(name); it != dataset_map.end()) {
            datasets11.emplace_back(it->second());
        } else {
            std::cerr << "Unknown dataset name: " << name << std::endl;
        }
    }

    CkksExperimentParams params{};

    for (auto i = 1; i <= epochs; i++) {
        for (auto j = 0; j < datasets11.size(); j++) {
            params.activation = TANH;
            params.approximation = CHEBYSHEV;
            params.epochs = i;
            params.earlyBootstrapping = 0;
            CkksNeuralNetworkExperiment("nn_ckks_tanh_chebyshev_" + std::to_string(params.epochs), *datasets11[j],
                                        params).Run();

            params.activation = TANH;
            params.approximation = TAYLOR;
            params.epochs = i;
            params.earlyBootstrapping = 0;
            CkksNeuralNetworkExperiment("nn_ckks_tanh_taylor_" + std::to_string(params.epochs), *datasets11[j],
                                        params).Run();

            params.activation = TANH;
            params.approximation = LEAST_SQUARES;
            params.epochs = i;
            params.earlyBootstrapping = 0;
            CkksNeuralNetworkExperiment("nn_ckks_tanh_least_squares_" + std::to_string(params.epochs), *datasets11[j],
                                        params).Run();

            params.activation = SIGMOID;
            params.approximation = CHEBYSHEV;
            params.epochs = i;
            params.earlyBootstrapping = 0;
            CkksNeuralNetworkExperiment("nn_ckks_sigmoid_chebyshev_" + std::to_string(params.epochs), *datasets11[j],
                                        params).Run();

            params.activation = SIGMOID;
            params.approximation = TAYLOR;
            params.epochs = i;
            params.earlyBootstrapping = 0;
            CkksNeuralNetworkExperiment("nn_ckks_sigmoid_taylor_" + std::to_string(params.epochs), *datasets11[j],
                                        params).Run();

            params.activation = SIGMOID;
            params.approximation = LEAST_SQUARES;
            params.epochs = i;
            params.earlyBootstrapping = 0;
            CkksNeuralNetworkExperiment("nn_ckks_sigmoid_least_squares_" + std::to_string(params.epochs),
                                        *datasets11[j],
                                        params).Run();
        }
    }
}
