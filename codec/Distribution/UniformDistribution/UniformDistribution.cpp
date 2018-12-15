#include "UniformDistribution.h"

UniformDistribution::UniformDistribution() {
}

void UniformDistribution::generate(std::string path) {
    std::ofstream out(path);

    for (unsigned char x = 0; x < UCHAR_MAX; x++) {
        double probability = 1.0 / static_cast<double>(UCHAR_MAX - 0);
        out << static_cast<int>(x) << " " << probability << std::endl;
    }

    out.close();
}

UniformDistribution::~UniformDistribution() {
}