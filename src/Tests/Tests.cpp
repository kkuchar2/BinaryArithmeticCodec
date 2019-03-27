#include "Tests.h"

void Tests::test1(int i) {
    // CREATE DISTRIBUTION
    //LaplaceDistribution * distribution = new LaplaceDistribution();
    //distribution->generate(0, 255, (unsigned char) i, "laplace_distribution.txt");
    //delete distribution;

    std::unique_ptr<NormalDistribution> distribution(new NormalDistribution());
    distribution->generate(static_cast<unsigned char>(128), "DISTRIBUTIONS/normal_distribution.txt");

    // CREATE ORIGINAL DATA
    std::unique_ptr<Data> data(new Data());
    data->generateFromDistribution("DISTRIBUTIONS/normal_distribution.txt", 100000);

    // SAVE ORIGINAL DATA TO FILE
    data->saveToFile("DATA/data.dat");

    // ENCODE ORIGINAL DATA TO NEW FILE
    std::unique_ptr<Encoder> encoder(new Encoder());
    encoder->encode("DATA/data.dat", "ENCODED/encoded_data.koda");

    // DECODE ENCODED DATA TO NEW FILE
    std::unique_ptr<Decoder> decoder(new Decoder());
    decoder->decodeData("ENCODED/encoded_data.koda", "DECODED/decoded_data.dat");

    // COMPARE DECODED DATA WITH ORIGINAL DATA

    // Read original data
    std::unique_ptr<Data> originalData(new Data());
    originalData->readFromFile("DATA/data.dat");

    // Read decoded data
    std::unique_ptr<Data> decodedData(new Data());
    decodedData->readFromFile("DECODED/decoded_data.dat");

    // Compare
    bool identical;

    try {
        identical = originalData->compare(decodedData);

        if (identical) {
            std::cout << "Decoded data is identical to original one :)";

            auto originalFileSize = Tests::filesize("DATA/data.dat");
            auto encodedFileSize = Tests::filesize("ENCODED/encoded_data.koda");
            auto ratio = 0.0;

            if (originalFileSize != 0) {
                ratio = 1.0 - static_cast<double>(encodedFileSize) / static_cast<double>(originalFileSize);
            }

            std::cout << "Compression ratio: " << ratio << std::endl;
        }
        else {
            std::cout << "Decoded data is different from original one :(" << std::endl;
            system("pause");
        }
    }
    catch (ErrorCodes) {
        std::cout << "Data size is not equal!" << std::endl;
        system("pause");
    }
}

void Tests::test2() {
    Image image;

    try {
        image.load("IMAGES/girl.jpg", IMAGE_GRAYSCALE);
    }
    catch (ErrorCodes e) {
        std::cout << e << std::endl;
    }

    auto pixels = image.getGray();
    auto encoder = new Encoder();

    encoder->encode(pixels, image.getWidth(), image.getHeight(), "ENCODED/girl.koda");

    delete encoder;

    auto decoder = new Decoder();

    auto decodedPixels = decoder->decodeImage("ENCODED/girl.koda", "DECODED/girl_decoded.png");

    std::cout << std::endl;

    // Check if equal
    auto same = true;
    for (auto i = 0; i < image.getWidth() * image.getHeight(); i++) {
        if (pixels[i] != decodedPixels[i]) {
            same = false;
            std::cout << i << " " << static_cast<int>(pixels[i]) << " " << static_cast<int>(decodedPixels[i]) << std::endl;
            break;
        }
    }

    if (!same) {
        std::cout << "Image data is not the same after decode! :(" << std::endl;
    }

    delete decoder;
    delete[] pixels;
    delete[] decodedPixels;
    system("pause");
}

void Tests::test3() {

}

void Tests::test4(int & argc, char * argv[]) {
    int numberOfParameters = argc - 1;

    if (numberOfParameters == 2) { // input_path + output_path
                                   // TODO: check if input and output path exist

        std::string inputPath = argv[1];
        std::string outputDir = argv[2];

        std::cout << "Input data path: " << inputPath << std::endl;
        std::cout << "Output directory: " << outputDir << std::endl;

        // Try to load image - if it's not an image - encode as data
        std::unique_ptr<Image> image(new Image());

        try {
            image->load(inputPath.c_str(), IMAGE_GRAYSCALE);
        }
        catch (ErrorCodes e) {
            if (e == CANNOT_OPEN_IMAGE) {
                std::cout << "Cannot open image, trying to encode as raw data..." << std::endl;

                std::unique_ptr<Encoder> encoder(new Encoder());

                // Get filename

                // std::string filename = getFilename(inputPath);

                //std::cout << "Filename: " << filename << std::endl;

                encoder->encode(inputPath, outputDir);
            }
            else if (e == IMAGE_CHANNELS_NOT_SUPPORTED) {
                std::cout << "Image channels are not supported!" << std::endl;
                exit(0);
            }
            else {
                std::cout << "Unexpected error!" << std::endl;
                exit(0);
            }
        }
    }

    std::cout << "argc = " << argc << std::endl;
    for (int i = 0; i < argc; i++)
        std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
}

long Tests::filesize(const char * filename) {
    std::ifstream in(filename, std::ifstream::ate | std::ifstream::binary);
    auto size = in.tellg();
    in.close();
    return static_cast<long>(size);
}