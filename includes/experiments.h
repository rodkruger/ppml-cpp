#ifndef EXPERIMENTS_H
#define EXPERIMENTS_H

#include "core.h"

namespace hermesml {
    class CkksPerceptronExperiment : public Experiment {
    public:
        explicit CkksPerceptronExperiment(const std::string &experimentId);

        void Run() override;
    };
}

#endif //EXPERIMENTS_H
