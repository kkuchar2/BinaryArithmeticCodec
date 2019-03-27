#pragma once

#include "../ArithmeticCodec/Encoder/Encoder.h"
#include "../ArithmeticCodec/Decoder/Decoder.h"

#include "../Distribution/UniformDistribution/UniformDistribution.h"
#include "../Distribution/NormalDistribution/NormalDistribution.h"
#include "../Distribution/LaplaceDistribution/LaplaceDistribution.h"

#include "../Utilities/Image/Image.h"
#include "../Data/Data.h"

class Tests {
    public:
        static void test1(int i);
        static void test2();
        static void test3();
        static void test4(int & argc, char * argv[]);

        static long filesize(const char * filename);
};