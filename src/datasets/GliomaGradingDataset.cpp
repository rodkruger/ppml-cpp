#include <fstream>

#include "datasets.h"

namespace hermesml {
    GliomaGradingDataset::GliomaGradingDataset(const DatasetRanges range) : Dataset("glioma_grading", range) {
    }
}
