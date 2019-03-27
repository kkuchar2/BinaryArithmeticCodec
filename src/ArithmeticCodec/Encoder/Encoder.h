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
#include <bitset>

class Encoder {
    private:
        // Buffer for sending bits to file
        unsigned char mBitBuffer = 0;
        // Sent bits counter
        unsigned char mBitCount = 0;

        // Output filestream
        std::ofstream mFile;

        // Write 8 bit variable to file
        static void WriteU8(std::ofstream & file, unsigned char value);

        // Write 32 bit variable to file
        static void WriteU32(std::ostream & file, unsigned int val);

        // Send integer bit (bit at position 'pos' in unsigned int) to file
        void SendIntegerBit(unsigned int bit, int pos);

        std::string sekwencja_bitow;
        long long dlugosc_sekwencji_bitow = 0;

        // Get data bit statistics
        static std::pair<std::vector<unsigned char>, std::pair<int, int>> getSymbolsInfo(std::string path);

        // Get pixel bit statistics
        static std::pair<std::vector<unsigned char>, std::pair<int, int>> getSymbolsInfo(unsigned char * pixels, int size);
        void encodeSymbols(const std::vector<unsigned char>& symbols, int zerosOccurences, int onesOccurences, std::string outputPath);
    public:
        Encoder();

        // Compress data from input file and save to output file
        void encode(std::string inputPath, std::string outputPath);

        // Compress image pixels and save to output file
        void encode(unsigned char * pixels, int width, int height, std::string outputPath);

        ~Encoder();
};