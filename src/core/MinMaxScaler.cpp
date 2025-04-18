#include "core.h"

namespace hermesml {
    MinMaxScaler::MinMaxScaler(const int8_t alpha, const int8_t beta) : alpha(alpha), beta(beta) {
    }

    void MinMaxScaler::Scale(std::vector<std::vector<double> > &data) const {
        const auto numRows = data.size();
        const auto numCols = data[0].size();

        for (size_t col = 0; col < numCols; ++col) {
            double minVal = data[0][col];
            double maxVal = data[0][col];

            for (size_t row = 1; row < numRows; ++row) {
                if (data[row][col] < minVal) {
                    minVal = data[row][col];
                }
                if (data[row][col] > maxVal) {
                    maxVal = data[row][col];
                }
            }

            for (size_t row = 0; row < numRows; ++row) {
                if (maxVal - minVal != 0) {
                    data[row][col] = this->alpha + (data[row][col] - minVal) * (
                                         (this->beta - this->alpha) / (maxVal - minVal));
                } else {
                    data[row][col] = 0.0;
                }
            }
        }
    }
}
