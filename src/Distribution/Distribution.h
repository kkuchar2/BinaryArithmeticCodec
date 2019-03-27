#pragma once

#include <vector>
#include <iostream>
#include <random>
#include <functional>
#include <algorithm>
#include <iterator>
#include <climits>
#include <fstream>
#include <chrono>
#include <cmath>

#include "../Utilities/ErrorCodes.h"

#define M_PI 3.14159265358979323846

class Distribution {
    private:
        std::vector<std::pair<unsigned char, double>> probabilities;
    public:
        Distribution();
        virtual ~Distribution();
};