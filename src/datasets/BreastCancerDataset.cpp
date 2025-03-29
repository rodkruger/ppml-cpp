#include <fstream>

#include "datasets.h"

namespace hermesml {
    BreastCancerDataset::BreastCancerDataset(const DatasetRanges range) : Dataset("breast_cancer", range) {
    }
}
