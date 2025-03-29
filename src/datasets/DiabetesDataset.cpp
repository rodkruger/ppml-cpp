#include <fstream>

#include "datasets.h"

namespace hermesml {
    DiabetesDataset::DiabetesDataset(const DatasetRanges range) : Dataset("diabetes", range) {
    }
}
