#include <fstream>

#include "datasets.h"

namespace hermesml {
    CreditCardFraudDataset::CreditCardFraudDataset(const DatasetRanges range) : Dataset("credit_card_fraud", range) {
    }
}
