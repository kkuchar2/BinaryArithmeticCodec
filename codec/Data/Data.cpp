#include "Data.h"

Data::Data() {
}

void Data::initWithArray(unsigned char * data, int size) {
    symbols.assign(data, data + size);
}

void Data::saveToFile(std::string path) {
    std::ofstream out(path, std::ofstream::binary);

    // Fill output file with chars generated from symbols
    for (size_t i = 0; i < symbols.size(); ++i) {
        out.write((char*) (&symbols[i]), sizeof(symbols[i]));
    }

    out.close();
}

void Data::readFromFile(std::string path) {
    std::ifstream f(path, std::ios::binary | std::ios::in);

    char c;
    while (f.get(c)) {
        symbols.push_back(static_cast<unsigned char>(c));
    }

    f.close();
}

void Data::generateFromDistribution(std::string path, int size) {
    if (!fileExists(path.c_str())) {
        throw FILE_DOES_NOT_EXIST;
    }

    // Read distribution weights from file
    std::ifstream infile(path);
    std::vector<double> weights;

    std::string line;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);

        int number;
        double probability;

        if (!(iss >> number >> probability)) {
            std::cout << "ERROR reading distribution!" << std::endl;
            break;
        }

        weights.push_back(probability);
    }

    // Generate random symbols based on gived distribution
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();

    // Gereator from seed
    std::mt19937 generator(static_cast<unsigned int>(seed));

    // Discrete distribution from generator
    std::discrete_distribution<int> distribution(weights.begin(), weights.end());

    // Fill symbols
    for (auto i = 0; i < size; i++) {
        symbols.push_back(static_cast<unsigned char>(distribution(generator)));
    }
}

bool Data::compare(const std::unique_ptr<Data> & data) {
    bool identical = true;

    // Check size
    if (this->getSize() != data->getSize()) {
        throw DATA_SIZE_NOT_EQUAL;
    }

    auto size = this->getSize();

    const std::vector<unsigned char> & otherSymbols = data->getSymbols();

    std::cout << "Comparing data..." << std::endl << std::endl;

    for (int i = 0; i < size; i++) {
        std::cout << "[" << i << "] First data: " << static_cast<int>(symbols[i]) << " Second data: " << static_cast<int>(otherSymbols[i]) << std::endl;
        if (symbols[i] != otherSymbols[i]) {
            identical = false;
            break;
        }
    }

    return identical;;
}

const std::vector<unsigned char> & Data::getSymbols() {
    return symbols;
}

size_t Data::getSize() {
    return symbols.size();
}

Data::~Data() {
}