#include "NormalDistribution.h"

NormalDistribution::NormalDistribution() {
}

void NormalDistribution::generate(unsigned char mean, std::string path) {
    // Mean
    unsigned char meanValue = mean;

    // Average half width of distribution
    double average_bound_width = ((meanValue - 0) + (UCHAR_MAX - meanValue)) / 2;

    // Standard deviation: 3x stddev rule = ~99.7% of distribution lies within range
    double const standard_deviation = average_bound_width / 3;

    double probability = 0.0;
    std::vector<double> probabilities;
    double sumOfProbabilities = 0.0;
    unsigned char x = 0;

    do {
        probability = (1.0 / (sqrt(2.0 * M_PI) * standard_deviation)) * exp(-pow(x - meanValue, 2) / (2 * pow(standard_deviation, 2)));
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

NormalDistribution::~NormalDistribution() {
}