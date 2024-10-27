#include "core.h"

namespace hermesml {

    std::vector<std::vector<int64_t> > Quantizer::Quantize(std::vector<std::vector<double> > &data) {

        auto outputData = std::vector<std::vector<int64_t> >();
        outputData.reserve(data.size());

        for (const std::vector<double>& row : data) {
            auto outputLine = std::vector<int64_t>();
            outputLine.reserve(row.size());

            for (const double value : row) {
                auto quantizedValue = static_cast<int64_t>(value * QUANTIZE_SCALE_FACTOR);
                outputLine.push_back(quantizedValue);
            }

            outputData.push_back(outputLine);
        }

        return outputData;
    }

}