#pragma once

#include "../Distribution.h"

class LaplaceDistribution : public Distribution {
    public:
        LaplaceDistribution();
        void generate(unsigned char mean, std::string path);
        ~LaplaceDistribution();
};