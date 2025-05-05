#include "model.h"

namespace hermesml {
    MlModel::MlModel(const uint32_t seed) : seed(seed) {
    }

    uint32_t MlModel::GetSeed() const {
        return this->seed;
    }
}
