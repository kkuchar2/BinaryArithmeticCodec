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

#include "../../Utilities/ErrorCodes.h"
#include "../../Utilities/Image/Image.h"

class Decoder {
    private:
        // Buffer for sending data bits to file
        unsigned int mBitBuffer = 0;

        // Buffer for sending pixel bits to array
        unsigned char singlePixelBuffer = 0;

        // Pixels array
        unsigned char * pixels = NULL;

        // Current index of pixel
        long index = 0;

        // Sent bits counter
        long long mBitCount = 0;

        // Output filestream
        std::ofstream mFile;

        // Write 8 bit to file
        void WriteU8(std::ostream & file, unsigned char val);

        // Save 8 bit to memory
        void SaveU8(unsigned char val);

        // Send bit to file
        void SendBit(const unsigned char bit, int pos);

        // Save bit in memory
        void SaveBit(const unsigned char bit, int pos);

        // Read zero bits from header
        int readZerosOccurences(std::ifstream & inputFileStream, int & readBytes);

        // Read positive bits count from header
        int readOnesOccurences(std::ifstream & inputFileStream, int & readBytes);

        // Read image width from header
        int readImageWidth(std::ifstream & inputFileStream, int & readBytes);

        // Read image height from header
        int readImageHeight(std::ifstream & inputFileStream, int & readBytes);

        // Read frist 16 bits from header of encoded data
        unsigned int readFirstCode(std::ifstream & inputFileStream, int & readBytes);

        // Read frist 16 bits from header of encoded image
        unsigned int readFirstImageCode(std::ifstream & inputFileStream, int & readBytes);

        // Decode pixels
        void decodeImagePixels(std::ifstream & inputFileStream, unsigned int CODE, int zerosOccurences, int onesOccurences, int & readBytes);

        // Decode symbols
        void decodeSymbols(std::ifstream & inputFileStream, unsigned int CODE, int zerosOccurences, int onesOccurences, int & readBytes, std::string outputPath);
    public:
        Decoder();

        // Decompress data from input file to output file
        void decodeData(std::string path, std::string outputPath);

        // Decompress image from input file to output file
        unsigned char * decodeImage(std::string path, std::string outputPath);

        ~Decoder();
};