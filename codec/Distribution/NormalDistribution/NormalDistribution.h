#pragma once

#include "../Distribution.h"

class NormalDistribution : public Distribution {
    public:
        NormalDistribution();
        void generate(unsigned char mean, std::string path);
        ~NormalDistribution();
};