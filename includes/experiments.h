#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include "core.h"

namespace hermesml {
    class CkksPerceptronExperiment : public Experiment {
        uint16_t epochs;

    public:
        explicit CkksPerceptronExperiment(const std::string &experimentId, uint16_t epochs);

        void Run() override;
    };
}

#endif //EXPERIMENTS_H
