#include <fstream>

#include "datasets.h"

namespace hermesml {
    LoanPredictionDataset::LoanPredictionDataset(const DatasetRanges range) : Dataset("loan_prediction", range) {
    }
}
