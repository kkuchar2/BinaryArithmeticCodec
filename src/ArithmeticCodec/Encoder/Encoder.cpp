#include "Encoder.h"

Encoder::Encoder() {
}

void Encoder::WriteU8(std::ofstream & file, unsigned char value) {
    file.write(reinterpret_cast<char*>(&value), sizeof(value));
}

void Encoder::WriteU32(std::ostream & file, unsigned int value) {
    // Extract the individual bytes from our value
    unsigned char firstByte  = ((value & 0xFF000000) >> 24);
    unsigned char secondByte = ((value & 0x00FF0000) >> 16);
    unsigned char thirdByte  = ((value & 0x0000FF00) >> 8);
    unsigned char fourthByte = (value & 0x000000FF);

    // write those bytes to the file
    file.write(reinterpret_cast<char*>(&firstByte),  sizeof(firstByte));
    file.write(reinterpret_cast<char*>(&secondByte), sizeof(secondByte));
    file.write(reinterpret_cast<char*>(&thirdByte),  sizeof(firstByte));
    file.write(reinterpret_cast<char*>(&fourthByte), sizeof(fourthByte));
}

void Encoder::SendIntegerBit(unsigned int bit, int pos) {
    // Reverse (so it become 0 or 1)
    unsigned int shiftedBit = (bit >> (sizeof(bit) * 8 - pos));

    // Move buffer so it will be place for new bit
    mBitBuffer <<= 1;

    // Write new bit at the end
    mBitBuffer |= shiftedBit;

    // Count written bits to buffer
    mBitCount++;

    // Count all sent bits
    dlugosc_sekwencji_bitow++;

    // Save bit to string buffer
    sekwencja_bitow.append(std::to_string(static_cast<int>(shiftedBit)));

    // If buffer is full, write to file and reset
    if (mBitCount == 8) {
        WriteU8(mFile, mBitBuffer);
        mBitCount = 0;
        mBitBuffer = 0;
    }
}

std::pair<std::vector<unsigned char>, std::pair<int, int>> Encoder::getSymbolsInfo(std::string path) {
    // Prepare vector of pairs: symbols and their statistics as pair of zero and one bit occurences
    std::pair<std::vector<unsigned char>, std::pair<int, int>> symbolsInfo;

    // Vector for input symbols (bits)
    std::vector<unsigned char> symbols;

    // Pair for zeros and ones occurences
    std::pair<int, int> occurences;
    auto zerosOccurences = 0;
    auto onesOccurences = 0;

    // Read input symbols (bits) and calculate their occurences
    std::ifstream f(path, std::ios::binary | std::ios::in);

    char c;
    while (f.get(c)) {
        for (auto i = 7; i >= 0; i--) {
            // Current symbol is current analysed bit
            unsigned char currentSymbol = ((c >> i) & 1);

            symbols.push_back(currentSymbol);

            if (currentSymbol == 0) {
                zerosOccurences++;
            }
            else if (currentSymbol == 1) {
                onesOccurences++;
            }
        }
    }

    f.close();

    occurences = std::make_pair(zerosOccurences, onesOccurences);
    return std::make_pair(symbols, occurences);
}

std::pair<std::vector<unsigned char>, std::pair<int, int>> Encoder::getSymbolsInfo(unsigned char * pixels, int size) {
    // Prepare vector of pairs: symbols and their statistics as pair of zero and one bit occurences
    std::pair<std::vector<unsigned char>, std::pair<int, int>> symbolsInfo;

    // Vector for input symbols (bits)
    std::vector<unsigned char> symbols;

    // Pair for zeros and ones occurences
    std::pair<int, int> occurences;

    auto zerosOccurences = 0;
    auto onesOccurences = 0;

    for (auto j = 0; j < size; j++) {
        for (auto i = 7; i >= 0; i--) {
            // Current symbol is current analysed bit
            unsigned char currentSymbol = ((pixels[j] >> i) & 1);

            symbols.push_back(currentSymbol);

            if (currentSymbol == 0) {
                zerosOccurences++;
            }
            else if (currentSymbol == 1) {
                onesOccurences++;
            }
        }
    }

    occurences = std::make_pair(zerosOccurences, onesOccurences);
    return std::make_pair(symbols, occurences);
}

void Encoder::encodeSymbols(const std::vector<unsigned char> & symbols, int zerosOccurences, int onesOccurences, std::string outputPath) {
    // Initialize encoder
    unsigned int LOWER_BOUND = 0x00000000; // Lower bound
    unsigned int UPPER_BOUND = 0x7FFFFFFF; // Upper bound
    long long OVERFLOW_COUNTER = 0; // Overflow counter

    // Encode symbol by symbol
    for (size_t i = 0; i < symbols.size(); i++) {
        // Calculate current range
        long long RANGE = static_cast<long long> (UPPER_BOUND - LOWER_BOUND) + 1;

        // Calculate range for first symbol
        unsigned int R1 = static_cast<unsigned int> (RANGE * zerosOccurences / (zerosOccurences + onesOccurences));

        if (symbols[i] == 0u) { // If current symbol (bit) is 0
            UPPER_BOUND = LOWER_BOUND + R1 - 1; // Upper bound gets lower bound + first symbol range + 1 (+ 1 is in case of rounding problem)
        }
        else { // If current symbol (bit) is 1
            LOWER_BOUND = LOWER_BOUND + R1; // Lower bound is lower bound + first_symbol_range
        }

        // Normalize
        for (;;) {
            // E1 / E2 (If MSB is the same)
            if ((UPPER_BOUND & 0x80000000) == (LOWER_BOUND & 0x80000000)) {
                // Send that bit to output file
                SendIntegerBit((UPPER_BOUND & 0x80000000), 1);

                // If overflow counter > 0, send that counter value bits, where every bit is negated upper bound MSB
                while (OVERFLOW_COUNTER > 0) {
                    SendIntegerBit(~UPPER_BOUND & 0x80000000, 1);
                    OVERFLOW_COUNTER--; // Decrease overflow counter
                }
            }
            // E3 (If second MSB is the same)
            else if ((LOWER_BOUND & 0x40000000) && !(UPPER_BOUND & 0x40000000)) {
                OVERFLOW_COUNTER += 1; // Increase overflow counter

                LOWER_BOUND &= 0x3fffffff;  // If MSB and second MSB of upper bound is 1 then becomes 0
                UPPER_BOUND |= 0x40000000; // if second MSB of lower bound is 0 then becomes 1
            }
            else {
                break;
            }

            LOWER_BOUND <<= 1;  // Shift lower bound left
            UPPER_BOUND <<= 1; // Shift upper bound left
            UPPER_BOUND |= 1;  // Set LSB of upper bound to 1
        }
    }

    // Send first bit of lower bound
    SendIntegerBit((LOWER_BOUND & 0x80000000), 1);

    // Send as many positive bits, as is the value of overflow counter
    while (OVERFLOW_COUNTER-- > 0) {
        SendIntegerBit(0x40000000, 2);
    }

    // Send remaining bits of lower bound
    LOWER_BOUND <<= 1;
    for (auto i = 0; i < 31; i++) {
        SendIntegerBit((LOWER_BOUND & 0x80000000), 1);
        LOWER_BOUND <<= 1;
    }

    // Calculate remaining bits to write (to fill last byte in buffer):
    unsigned char liczba_bitow_do_uzupelnienia = static_cast<unsigned char>(8 - (dlugosc_sekwencji_bitow - (dlugosc_sekwencji_bitow / 8) * 8));

    // Fill remaining bits in last byte in buffer with zeros
    for (auto i = 0; i < liczba_bitow_do_uzupelnienia; i++) {
        SendIntegerBit(0x00000000, 1);
    }
}

void Encoder::encode(std::string inputPath, std::string outputPath) {
    // Pobierz symbole i informacjê o nich
    auto symbolsInfo = getSymbolsInfo(inputPath);

    // Wektor na symbole wejœciowe
    auto symbols = symbolsInfo.first;

    // Liczba zer
    auto zerosOccurences = symbolsInfo.second.first;

    // Liczba jedynek
    auto onesOccurences = symbolsInfo.second.second;

    // Entropia danych
    auto p0 = static_cast<double>(zerosOccurences) / static_cast<double>(zerosOccurences + onesOccurences);
    auto p1 = static_cast<double>(onesOccurences) / static_cast<double>(zerosOccurences + onesOccurences);
    auto H = -(p0 * log10(p0) + p1 * log10(p1));

    // Otwórz plik wyjœciowy
    mFile.open(outputPath, std::ios::binary | std::ios::out);

    // Zapisz w nag³ówku 4 bajty dla liczby ca³kowitej opisuj¹cej wyst¹pienia zer
    WriteU32(mFile, zerosOccurences);

    // Zapisz w nag³ówku 4 bajty dla liczby ca³kowitej opisuj¹cej wyst¹pienia jedynek
    WriteU32(mFile, onesOccurences);

    // Koduj symbole
    encodeSymbols(symbols, zerosOccurences, onesOccurences, outputPath);

    // Zamknij plik wyjœciowy
    mFile.close();
}

void Encoder::encode(unsigned char * pixels, int width, int height, std::string outputPath) {
    // Pobierz symbole i informacjê o nich
    //std::cout << std::endl << "Wyliczanie statystyki bitow wejsciowych..." << std::endl << std::endl;
    auto size = width * height;
    auto symbolsInfo = getSymbolsInfo(pixels, size);

    // Wektor na symbole wejœciowe
    auto symbols = symbolsInfo.first;

    // Liczba zer
    auto zerosOccurences = symbolsInfo.second.first;

    // Liczba jedynek
    auto onesOccurences = symbolsInfo.second.second;

    //std::cout << std::endl << "Wyliczanie entropii..." << std::endl;

    // Entropia danych
    auto p0 = static_cast<double>(zerosOccurences) / static_cast<double>(zerosOccurences + onesOccurences);
    auto p1 = static_cast<double>(onesOccurences) / static_cast<double>(zerosOccurences + onesOccurences);
    auto H = -(p0 * log10(p0) + p1 * log10(p1));

    //std::cout << std::endl << "Entropia H = " << H << std::endl;

    // Otwórz plik wyjœciowy
    mFile.open(outputPath, std::ios::binary | std::ios::out);

    // Zapisz w nag³ówku 4 bajty dla liczby ca³kowitej opisuj¹cej wyst¹pienia zer
    //std::cout << std::endl << "Zakodowanie liczby zer (" << zerosOccurences << ") w naglowku..." << std::endl;
    WriteU32(mFile, zerosOccurences);

    // Zapisz w nag³ówku 4 bajty dla liczby ca³kowitej opisuj¹cej wyst¹pienia jedynek
    //std::cout << std::endl << "Zakodowanie liczby jedynek (" << onesOccurences << ") w naglowku..." << std::endl;
    WriteU32(mFile, onesOccurences);

    // Zapisz w nag³ówku wysokosæ i szerokoœæ obrazu
    //std::cout << std::endl << "Zakodowanie szerokosci (" << width << " pix) i " << "wysokosci (" << height << " pix) obrazu w naglowku..." << std::endl;
    WriteU32(mFile, width);
    WriteU32(mFile, height);

    // Koduj symbole
    encodeSymbols(symbols, zerosOccurences, onesOccurences, outputPath);

    // Zamknij plik wyjœciowy
    mFile.close();
}

Encoder::~Encoder() {
}