#include "Decoder.h"

Decoder::Decoder() {
}

void Decoder::WriteU8(std::ostream & file, unsigned char val) {
    file.write(reinterpret_cast<char*>(&val), sizeof(val));
}

void Decoder::SaveU8(unsigned char val) {
    pixels[index] = val;
    index++;
}

void Decoder::SendBit(const unsigned char bit, int pos) {
    // Write bit 
    unsigned char shiftedBit = (bit >> (sizeof(bit) * 8 - pos));

    mBitBuffer <<= 1;
    mBitBuffer |= shiftedBit;

    // Count written bits to buffer
    mBitCount++;

    // If buffer is full, write to file and reset
    if (mBitCount == 8) {
        WriteU8(mFile, mBitBuffer);
        mBitCount = 0;
        mBitBuffer = 0;
    }
}

void Decoder::SaveBit(const unsigned char bit, int pos) {
    // Write bit 
    unsigned char shiftedBit = (bit >> (sizeof(bit) * 8 - pos));

    singlePixelBuffer <<= 1;
    singlePixelBuffer |= shiftedBit;

    // Count written bits to buffer
    mBitCount++;

    // If buffer is full, write to file and reset
    if (mBitCount == 8) {
        SaveU8(singlePixelBuffer);
        mBitCount = 0;
        singlePixelBuffer = 0;
    }
}

int Decoder::readZerosOccurences(std::ifstream & inputFileStream, int & readBytes) {
    int zerosOccurences = 0;

    unsigned char currentReadByte;
    while (readBytes < 4) {
        currentReadByte = inputFileStream.get();
        readBytes++;

        if (currentReadByte != EOF) {
            zerosOccurences |= (currentReadByte << (8 * (4 - readBytes)));
        }
        else {
            throw UNEXPECTED_EOF;
        }
    }

    return zerosOccurences;
}

int Decoder::readOnesOccurences(std::ifstream & inputFileStream, int & readBytes) {
    int onesOccurences = 0;

    unsigned char currentReadByte;
    while (readBytes < 8) {
        currentReadByte = inputFileStream.get();
        readBytes++;

        if (currentReadByte != EOF) {
            onesOccurences |= (currentReadByte << (8 * (8 - readBytes)));
        }
        else {
            throw UNEXPECTED_EOF;
        }
    }
    return onesOccurences;
}

int Decoder::readImageWidth(std::ifstream & inputFileStream, int & readBytes) {
    int imageWidth = 0;

    unsigned char currentReadByte;
    while (readBytes < 12) {
        currentReadByte = inputFileStream.get();
        readBytes++;

        if (currentReadByte != EOF) {
            imageWidth |= (currentReadByte << (8 * (12 - readBytes)));
        }
        else {
            throw UNEXPECTED_EOF;
        }
    }
    return imageWidth;
}

int Decoder::readImageHeight(std::ifstream & inputFileStream, int & readBytes) {
    unsigned int imageHeight = 0;

    unsigned char currentReadByte;
    while (readBytes < 16) {
        currentReadByte = inputFileStream.get();
        readBytes++;

        if (currentReadByte != EOF) {
            unsigned int curr = static_cast<unsigned int>(currentReadByte);
            imageHeight |= (curr << (8 * (16 - readBytes)));
        }
        else {
            throw UNEXPECTED_EOF;
        }
    }
    return imageHeight;
}

unsigned int Decoder::readFirstCode(std::ifstream & inputFileStream, int & readBytes) {
    unsigned char currentReadByte;

    unsigned int CODE = 0;

    while (readBytes < 12) {
        currentReadByte = inputFileStream.get();
        readBytes++;

        if (currentReadByte != EOF) {
            for (int i = 7; i >= 0; i--) {
                // Read current bit of current byte
                unsigned char currentBit = ((currentReadByte >> i) & 1);

                // Read first 32 bits to variable KOD
                CODE <<= 1;
                CODE += currentBit;
            }
        }
        else {
            throw UNEXPECTED_EOF;
        }
    }
    return CODE;
}

unsigned int Decoder::readFirstImageCode(std::ifstream & inputFileStream, int & readBytes) {
    unsigned char currentReadByte;

    unsigned int CODE = 0;

    while (readBytes < 20) {
        currentReadByte = inputFileStream.get();
        readBytes++;

        if (currentReadByte != EOF) {
            for (int i = 7; i >= 0; i--) {
                // Read current bit of current byte
                unsigned char currentBit = ((currentReadByte >> i) & 1);

                // Read first 32 bits to variable KOD
                CODE <<= 1;
                CODE += currentBit;
            }
        }
        else {
            throw UNEXPECTED_EOF;
        }
    }
    return CODE;
}

void Decoder::decodeImagePixels(std::ifstream & inputFileStream, unsigned int CODE, int zerosOccurences, int onesOccurences, int & readBytes) {
    // Initialize encoder
    unsigned int LOWER_BOUND = 0x00000000;  // Lower bound
    unsigned int UPPER_BOUND = 0x7FFFFFFF; // Upper bound
    int LS = 0; // Decoded symbols counter

    int i = -1;
    unsigned char currentReadByte = 0;

    while (LS < zerosOccurences + onesOccurences) {
        // Update ranges
        long long RANGE = static_cast<long long>(UPPER_BOUND - LOWER_BOUND) + 1;

        // Calculate range for first symbol
        unsigned int R1 = static_cast<unsigned int>(RANGE * zerosOccurences / (zerosOccurences + onesOccurences));

        if (CODE - LOWER_BOUND < R1) {
            SaveBit(0, sizeof(unsigned char) * 8);
            UPPER_BOUND = LOWER_BOUND + R1 - 1;
        }
        else {
            SaveBit(1, sizeof(unsigned char) * 8);
            LOWER_BOUND = LOWER_BOUND + R1;
        }

        LS++;

        // Normalize
        for (;;) {
            if ((LOWER_BOUND & 0x80000000) == (UPPER_BOUND & 0x80000000)) {
            }
            else if ((LOWER_BOUND & 0x40000000) && !(UPPER_BOUND & 0x40000000)) {
                CODE ^= 0x40000000;
                LOWER_BOUND &= 0x3fffffff;
                UPPER_BOUND |= 0x40000000;
            }
            else {
                break;
            }

            LOWER_BOUND <<= 1;
            UPPER_BOUND <<= 1;
            CODE <<= 1;
            UPPER_BOUND |= 1;

            // If next bit position went to next byte, get next byte and reset bit position to first one
            if (i == -1) {
                currentReadByte = inputFileStream.get();

                if (currentReadByte == EOF) {
                    throw UNEXPECTED_EOF;
                }

                readBytes++;
                i = 7;
            }

            // Read next bit of current byte
            unsigned char nextBit = ((currentReadByte >> i) & 1);
           
            CODE += nextBit;

            // Set bit position to next bit
            i--;
        }
    }
}

void Decoder::decodeSymbols(std::ifstream & inputFileStream, unsigned int CODE, int zerosOccurences, int onesOccurences, int & readBytes, std::string outputPath) {
    // Initialize encoder
    unsigned int LOWER_BOUND = 0x00000000;  // Lower bound
    unsigned int UPPER_BOUND = 0x7FFFFFFF; // Upper bound
    int DECODED_SYMBOLS = 0; // Decoded symbols counter

    int i = -1;
    unsigned char currentReadByte = 0;

    // Open output file
    mFile.open(outputPath, std::ios::binary | std::ios::out);

    // Decode symbols
    while (DECODED_SYMBOLS < zerosOccurences + onesOccurences) {
        // Calculate current range
        long long RANGE = static_cast<long long>(UPPER_BOUND - LOWER_BOUND) + 1;

        // Calculate range for first symbol
        unsigned int R1 = static_cast<unsigned int>(RANGE * zerosOccurences / (zerosOccurences + onesOccurences));

        // Decode symbol
        if (CODE - LOWER_BOUND < R1) {
            SendBit(0, sizeof(unsigned char) * 8);
            UPPER_BOUND = LOWER_BOUND + R1 - 1;
        }
        else {
            SendBit(1, sizeof(unsigned char) * 8);
            LOWER_BOUND = LOWER_BOUND + R1;
        }

        DECODED_SYMBOLS++;

        // Normalize
        for (;;) {
            if ((LOWER_BOUND & 0x80000000) == (UPPER_BOUND & 0x80000000)) {
            }
            else if ((LOWER_BOUND & 0x40000000) == 0X40000000 && (UPPER_BOUND & 0x40000000) == 0) {
                CODE ^= 0x40000000;
                LOWER_BOUND &= 0x3fffffff;
                UPPER_BOUND |= 0x40000000;
            }
            else {
                break;
            }

            LOWER_BOUND <<= 1;
            UPPER_BOUND <<= 1;
            CODE <<= 1;
            UPPER_BOUND |= 1;

            // If next bit position went to next byte, get next byte and reset bit position to first one
            if (i == -1) {
                currentReadByte = inputFileStream.get();

                if (currentReadByte == EOF) {
                    throw UNEXPECTED_EOF;
                }

                readBytes++;
                i = 7;
            }

            // Read next bit of current byte
            unsigned char nextBit = ((currentReadByte >> i) & 1);
            CODE += nextBit;

            // Set bit position to next bit
            i--;
        }
    }
    mFile.close();
}

void Decoder::decodeData(std::string path, std::string outputPath) {
    // Read all numbers from file bit by bit, byte by byte and get statistics
    std::ifstream inputFileStream(path, std::ios::binary | std::ios::in);

    int zerosOccurences = 0;
    int onesOccurences = 0;

    // Count read bytes
    int readBytes = 0;

    // Read zeros occurences
    try {
        zerosOccurences = readZerosOccurences(inputFileStream, readBytes);
    }
    catch (ErrorCodes e) {
        throw e;
    }

    //std::cout << "Deocoded zeros occurences: " << zerosOccurences << std::endl;

    // Read ones occurences
    try {
        onesOccurences = readOnesOccurences(inputFileStream, readBytes);
    }
    catch (ErrorCodes e) {
        throw e;
    }

    //std::cout << "Deocoded ones occurences: " << onesOccurences << std::endl;

    // Read first code
    unsigned int KOD = 0;
    try {
        KOD = readFirstCode(inputFileStream, readBytes);
    }
    catch (ErrorCodes e) {
        throw e;
    }

    // Decode symbols and save decoded data to new file
    try {
        decodeSymbols(inputFileStream, KOD, zerosOccurences, onesOccurences, readBytes, outputPath);
    }
    catch (ErrorCodes e) {
        throw e;
    }

    inputFileStream.close();
}

unsigned char * Decoder::decodeImage(std::string path, std::string outputPath) {
    //std::cout << "------------------------------------------------------------------------------" << std::endl;
    //std::cout << "Praca dekodera rozpoczeta" << std::endl;
    //std::cout << "------------------------------------------------------------------------------" << std::endl;

    std::ifstream inputFileStream(path, std::ios::binary | std::ios::in);

    int zerosOccurences = 0;
    int onesOccurences = 0;
    int imageWidth = 0;
    int imageHeight = 0;

    // Count read bytes
    int readBytes = 0;

    // Read zeros occurences
    try {
        zerosOccurences = readZerosOccurences(inputFileStream, readBytes);
    }
    catch (ErrorCodes e) {
        throw e;
    }

    //std::cout << std::endl << "Zdekodowana liczba zer: " << zerosOccurences << std::endl;

    // Read ones occurences
    try {
        onesOccurences = readOnesOccurences(inputFileStream, readBytes);
    }
    catch (ErrorCodes e) {
        throw e;
    }

    //std::cout << std::endl << "Zdekodowana liczba jedynek: " << onesOccurences << std::endl;

    // Read image width
    try {
        imageWidth = readImageWidth(inputFileStream, readBytes);
        //std::cout << std::endl << "Zdekodowana szerokosc obrazu: " << imageWidth;
    }
    catch (ErrorCodes e) {
        throw e;
    }

    // Read image height
    try {
        imageHeight = readImageHeight(inputFileStream, readBytes);
        //std::cout << std::endl << "Zdekodowana wysokosc obrazu: " << imageHeight << std::endl;
    }
    catch (ErrorCodes e) {
        throw e;
    }

    // Read first code
    unsigned int KOD = 0;
    try {
        KOD = readFirstImageCode(inputFileStream, readBytes);
        //std::cout << std::endl << "Zdekodowany 32 bitowy KOD: " << KOD << std::endl;
    }
    catch (ErrorCodes e) {
        throw e;
    }

    // Decode symbols and save decoded data to new file
    this->pixels = new unsigned char[imageWidth * imageHeight];

    try {
        decodeImagePixels(inputFileStream, KOD, zerosOccurences, onesOccurences, readBytes);
    }
    catch (ErrorCodes e) {
        throw e;
    }

    // Create decoded image
    //std::cout << imageHeight << " " << imageWidth << std::endl;
    cv::Mat mat(imageHeight, imageWidth, CV_8UC1);

    int i = 0;
    for (int y = 0; y < imageWidth; y++) {
        for (int x = 0; x < imageHeight; x++) {
            mat.at<uchar>(x, y) = pixels[i];
            i++;
        }
    }

    // Save image to file
    cv::imwrite(outputPath, mat);

    //std::cout << std::endl;
    //std::cout << "------------------------------------------------------------------------------" << std::endl;
    //std::cout << "Praca dekodera zakonczona." << std::endl;
    //std::cout << "------------------------------------------------------------------------------" << std::endl;
    //std::cout << std::endl;

    return pixels;
}

Decoder::~Decoder() {
}