#include "datasets.h"
#include "experiments.h"

using namespace hermesml;

int main(const int argc, char *argv[]) {
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

    if (datasets11.empty()) {
        datasets11.emplace_back(dataset_map["diabetes"]());
        datasets11.emplace_back(dataset_map["breast"]());
        datasets11.emplace_back(dataset_map["glioma"]());
        datasets11.emplace_back(dataset_map["thyroid"]());
        datasets11.emplace_back(dataset_map["cirrhosis"]());
    }

    struct {
        ActivationFn activation;
        ApproximationFn approximation;
        std::string name;
    } experiments[] = {
                {TANH, CHEBYSHEV, "nn_ckks_tanh_chebyshev"},
                {TANH, TAYLOR, "nn_ckks_tanh_taylor"},
                {TANH, LEAST_SQUARES, "nn_ckks_tanh_least_squares"},
                {SIGMOID, CHEBYSHEV, "nn_ckks_sigmoid_chebyshev"},
                {SIGMOID, TAYLOR, "nn_ckks_sigmoid_taylor"},
                {SIGMOID, LEAST_SQUARES, "nn_ckks_sigmoid_least_squares"}
            };

    CkksExperimentParams params{};

    params.epochs = 30;
    params.earlyBootstrapping = 3;

    for (auto j = 0; j < datasets11.size(); j++) {
        for (const auto &[activation, approximation, name]: experiments) {
            params.activation = activation;
            params.approximation = approximation;

            try {
                CkksNeuralNetworkExperiment(name, *datasets11[j], params).Run();
            } catch (const OpenFHEException &e) {
                std::cerr << "[OpenFHEException] " << name << " failed on dataset " << j
                        << ": " << e.what() << std::endl;
            }
        }
    }
}
