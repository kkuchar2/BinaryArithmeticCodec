#include "ArithmeticCodec/Encoder/Encoder.h"
#include "ArithmeticCodec/Decoder/Decoder.h"

#include "Distribution/UniformDistribution/UniformDistribution.h"
#include "Distribution/NormalDistribution/NormalDistribution.h"
#include "Distribution/LaplaceDistribution/LaplaceDistribution.h"

#include "Utilities/Image/Image.h"
#include "Data/Data.h"

void generateUniformDistribution(std::string & outputDistributionFile) {
    std::unique_ptr<UniformDistribution> distribution(new UniformDistribution());
    distribution->generate(outputDistributionFile);
}

void generateNormalDistribution(std::string & outputDistributionFile, char * argv[], int & numberOfParameters) {
    // Default mean
    if (numberOfParameters == 4) {
        std::unique_ptr<NormalDistribution> distribution(new NormalDistribution());
        distribution->generate(128, outputDistributionFile);
    }
    // User definied mean
    else if (numberOfParameters == 5) {
        unsigned char mean = static_cast<unsigned char>(std::stoi(argv[5]));
        std::unique_ptr<NormalDistribution> distribution(new NormalDistribution());
        distribution->generate(mean, outputDistributionFile);
    }
}

void generateLaplaceDistribution(std::string & outputDistributionFile, char * argv[], int & numberOfParameters) {
    std::unique_ptr<LaplaceDistribution> distribution(new LaplaceDistribution());

    if (numberOfParameters == 4) {
        // Default mean
        distribution->generate(128, outputDistributionFile);
    }
    else if (numberOfParameters == 5) {
        // User definied mean
        unsigned char mean = static_cast<unsigned char>(std::stoi(argv[5]));
        distribution->generate(mean, outputDistributionFile);
    }
}

void generateRawData(std::string & distributionFile, std::string & outputDataFile, std::string & numberOfSamples) {
    std::unique_ptr<Data> data(new Data());
    data->generateFromDistribution(distributionFile, std::stoi(numberOfSamples));
    data->saveToFile(outputDataFile);
}

void encodeRawData(std::string & inputPath, std::string & outputPath) {
    std::unique_ptr<Encoder> encoder(new Encoder());
    encoder->encode(inputPath, outputPath);
}

void encodeImage(std::string & inputPath, std::string & outputPath) {
    std::unique_ptr<Image> image(new Image());
    image->load(inputPath.c_str(), IMAGE_GRAYSCALE);
    auto pixels = image->getGray();
    std::unique_ptr<Encoder> encoder(new Encoder());
    encoder->encode(pixels, image->getWidth(), image->getHeight(), outputPath);
}

void decodeRawData(std::string & inputPath, std::string & outputPath) {
    std::unique_ptr<Decoder> decoder(new Decoder());
    decoder->decodeData(inputPath, outputPath);
}

void decodeImage(std::string & inputPath, std::string & outputPath) {
    std::unique_ptr<Decoder> decoder(new Decoder());
    decoder->decodeImage(inputPath, outputPath);
}

void process(int & argc, char * argv[]) {
    /*
    ##########################################################################################
    generator distribution [distribution_type] [mean (optional)] where distribution_type = uniform | normal | laplace
    generator data [distribution_file_path] [output_path] [number_of_samples]

    coder   [input_path] [output_path] [mode]  where mode = data | image
    decoder [input_path] [output_path] [mode]  where mode = data | image
    ############################################################################################
    */

    // Number of passed parameters
    auto numberOfParameters = argc - 1;

    if (numberOfParameters < 2) {
        throw WRONG_NUMBER_OF_PARAMETERS;
    }

    // Process first parameter [coder | decoder | generator]
    std::string firstParameter = argv[1];

    if (firstParameter.compare("coder") == 0) {
        if (numberOfParameters != 4) {
            throw WRONG_NUMBER_OF_PARAMETERS;
        }

        std::string inputPath = argv[2];
        std::string outputPath = argv[3];
        std::string mode = argv[4];

        // Check if input data file or image exists
        if (!Data::fileExists(inputPath.c_str())) {
            throw FILE_DOES_NOT_EXIST;
        }

        // Encode data or image
        mode.compare("data") == 0 ? encodeRawData(inputPath, outputPath) :
            mode.compare("image") == 0 ? encodeImage(inputPath, outputPath) : throw UNRECOGNIZED_MODE;
    }
    else if (firstParameter.compare("decoder") == 0) {
        if (numberOfParameters != 4) {
            throw WRONG_NUMBER_OF_PARAMETERS;
        }

        std::string inputPath = argv[2];
        std::string outputPath = argv[3];
        std::string mode = argv[4];

        // Check if input encoded file exists
        if (!Data::fileExists(inputPath.c_str())) {
            throw FILE_DOES_NOT_EXIST;
        }

        // Decode to data or to image
        mode.compare("data") == 0 ? decodeRawData(inputPath, outputPath) :
            mode.compare("image") == 0 ? decodeImage(inputPath, outputPath) : throw UNRECOGNIZED_MODE;
    }
    else if (firstParameter.compare("generator") == 0) {
        std::string mode = argv[2];

        if (mode.compare("distribution") == 0) {
            if (numberOfParameters < 4) {
                throw WRONG_NUMBER_OF_PARAMETERS;
            }

            // Get distribution type and outpu distribution file path
            std::string distributionType = argv[3];
            std::string outputDistributionFile = argv[4];

            // Generate distribution
            distributionType.compare("uniform") == 0 ? generateUniformDistribution(outputDistributionFile)                          :
            distributionType.compare("normal")  == 0 ? generateNormalDistribution(outputDistributionFile, argv, numberOfParameters) :
            distributionType.compare("laplace") == 0 ? generateLaplaceDistribution(outputDistributionFile, argv, numberOfParameters)
                                                     : throw WRONG_DISTRIBUTION_TYPE;
        }
        else if (mode.compare("data") == 0) {
            if (numberOfParameters < 5) {
                throw WRONG_NUMBER_OF_PARAMETERS;
            }

            // Get distribution type, output distribution file pathand number of samples
            std::string distributionFile = argv[3];
            std::string outputDataFile = argv[4];
            std::string numberOfSamples = argv[5];

            // Generate data from distribution file
            generateRawData(distributionFile, outputDataFile, numberOfSamples);
        }
        else {
            throw WRONG_TYPE_OF_GENERATION;
        }
    }
    else {
        throw WRONG_FIRST_PARAMETER;
    }
}

int main(int argc, char* argv[]) {
    try {
        process(argc, argv);
    }
    catch (ErrorCodes e) {
        if (e == WRONG_NUMBER_OF_PARAMETERS) {
            std::cout << "Niepoprawna liczba parametrow" << std::endl;
        }
        else if (e == WRONG_FIRST_PARAMETER) {
            std::cout << "Niepoprawny pierwszy parametr" << std::endl;
        }
        else if (e == FILE_DOES_NOT_EXIST) {
            std::cout << "Plik wejsciowy jest uszkodzony lub nie istnieje." << std::endl;
        }
        else if (e == UNRECOGNIZED_MODE) {
            std::cout << "Nie rozpoznano trybu operacji" << std::endl;
        }
        else if (e == WRONG_TYPE_OF_GENERATION) {
            std::cout << "Nieprawidlowy typ generacji" << std::endl;
        }
        else if (e == WRONG_DISTRIBUTION_TYPE) {
            std::cout << "NIeprawid³owy typ rozk³adu prawdopodobienstwa" << std::endl;
        }
        exit(static_cast<int>(e));
    }
}