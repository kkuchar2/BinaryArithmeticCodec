#pragma once

#include "../Distribution.h"

class UniformDistribution : public Distribution {
    public:
        UniformDistribution();
        void generate(std::string path);
        ~UniformDistribution();
};