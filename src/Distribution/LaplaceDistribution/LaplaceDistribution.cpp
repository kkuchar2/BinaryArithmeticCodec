#include "LaplaceDistribution.h"

LaplaceDistribution::LaplaceDistribution() {
}

void LaplaceDistribution::generate(unsigned char mean, std::string path) {
    double meanValue = static_cast<double>(mean);

    // Average half width of distribution
    double average_bound_width = ((meanValue - 0) + (UCHAR_MAX - meanValue)) / 2;

    // Standard deviation: 5x stddev rule = ~99.7% of distribution lies within range
    double const lambda = average_bound_width / 5;

    unsigned char x = 0;
    double probability = 0.0;

    std::vector<double> probabilities;
    double sumOfProbabilities = 0.0;
    do {
        probability = 1.0 / (2.0 * lambda) * exp(-abs(static_cast<double>(x) - meanValue) / lambda);
        probabilities.push_back(probability);
        sumOfProbabilities += probability;
    }
    while (++x);

    std::ofstream out(path);

    // Normalize probabilities to range and save to file
    for (size_t i = 0; i < probabilities.size(); i++) {
        out << static_cast<int>(i) << " " << probabilities[i] / sumOfProbabilities << std::endl;
    }

    out.close();
}

LaplaceDistribution::~LaplaceDistribution() {
}