#pragma once

#include <iostream>
#include <random>
#include <functional>
#include <algorithm>
#include <iterator>
#include <climits>
#include <fstream>
#include <vector>
#include <chrono>
#include <sstream>
#include <string>
#include <memory>
#include <sys/stat.h>

#include "../Utilities/ErrorCodes.h"

class Data {
    protected:
        // Symbols stored in data
        std::vector<unsigned char> symbols;
    public:
        Data();

        // Check if file ezists
        static int fileExists(const char * filename) {
            struct stat buffer;
            return (stat(filename, &buffer) == 0);
        }

        // Initialize data with array
        void initWithArray(unsigned char * data, int size);

        // Generate data from distribution file
        void generateFromDistribution(std::string path, int size);

        // Save data to file
        void saveToFile(std::string path);

        // Read data from file
        void readFromFile(std::string path);

        // Compare two data objects
        bool compare(const std::unique_ptr<Data> & data);

        // Get symbols
        const std::vector<unsigned char> & getSymbols();

        // Get size
        size_t getSize();

        ~Data();
};
