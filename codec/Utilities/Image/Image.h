#ifndef _IMAGE_H 
#define _IMAGE_H 

#include <iostream>
#include <memory>
#include <iostream>
#include <stdio.h>
#include <malloc.h>
#include "opencv2/opencv.hpp"

#include "../ErrorCodes.h"

enum LoadMode {
	IMAGE_UNCHANGED = 1, //!< image is loaded in unchanged way
	IMAGE_COLOR     = 2, //!< image is loaded, as it has 3 channels (R,G,B)
	IMAGE_GRAYSCALE = 3, //!< image is loaded in grayscale, OpenCV is loading grayscale using luminosity method (not average)
};

enum GrayscaleMode {
    GRAYSCALE_LUMINOSITY = 1, //!< Luminosity method for accessing grayscale values
    GRAYSCALE_AVERAGE    = 2  //!< Average method for accessing grayscale values
};

class Image {
	private:
		cv::Mat mat;					  // cv::Mat object used to store image data

		int imageWidth;					  // Image width
		int imageHeight;			      // Image height
		int imageSize;					  // Image size (width * height)
		int totalImageSize;				  // Total image size (number of channels * imageSize)
		bool transparencyPresent = false; // Variable indicating presence of alpha (transparency) channel

		int maxAlpha = -1;				  // Maximum value of alpha channel
		int minAlpha = -1;				  // Minimum value of alpha channel

        uchar channels;                   // Number of channels
        uchar depth;                      // Datatype
	public:
		Image();		                  // Constructor creating empty image object				  

        /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
        /* Image loading
        /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

		// Method loading image decoding from 'data' array with mode specified in decode_mode
        void load(unsigned char * data, int size, LoadMode decode_mode);
        // Method loading image located at 'filename' path with mode specified in load_mode
        void load(const char * filename, LoadMode load_mode);
		
        /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
        /* Basic functions
        /* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

        /** @return cv::Mat - OpenCV cv::mat object with image data */
		cv::Mat & getMat();	

        /** @return int - image width property in pixels */
		int getWidth();

        /** @return int - image height property in pixels */
		int getHeight();

        /** @return int - image size (Width * height) property in pixels */
		int getSize();

        /** @return int - image total size (Width * height * number of channels) in pixels */
		int getTotalSize();

        /** @return bool - image transparency presence indicator */
		bool getTransparencyPresent();

        /** @brief
        * Method returning G (green mask) channel for an image.       \n\n
        * If an image is RGBA (4 channels), returns index 3           \n
        * If an image is RGB (3 channels), returns index 2            \n
        * If an image is GrayscaleAlpha (2 channels), returns index 1 \n
        * If an image is Grayscale (1 channel), returns index 0       \n
        *
        * @return unsigned char * - pointer to mask array */

		unsigned char * getChannel_R();

        /** @brief
        * Method returning G (green mask) channel for an image.       \n\n
        * If an image is RGBA (4 channels), returns index 2           \n
        * If an image is RGB (3 channels), returns index 1            \n
        * If an image is GrayscaleAlpha (2 channels), returns index 1 \n
        * If an image is Grayscale (1 channel), returns index 0       \n
        *
        * @return unsigned char * - pointer to mask array */

		unsigned char * getChannel_G();

        /** @brief
        * Method returning B (blue mask) channel for an image.        \n\n
        * If an image is RGBA (4 channels), returns index 1           \n
        * If an image is RGB (3 channels), returns index 0            \n
        * If an image is GrayscaleAlpha (2 channels), returns index 0 \n
        * If an image is Grayscale (1 channel), returns index 0       \n
        * 
        * @return unsigned char * - pointer to mask array */

		unsigned char * getChannel_B();

        /** @brief
        * Method returning A (transparency mask) channel for an image. \n\n
        * If an image is RGBA (4 channels), return index 0             \n
        * If an image is GrayscaleAlpha (2 channels), returns index 0  \n
        *
        * @return unsigned char * - pointer to mask array */

		unsigned char * getChannel_A();

        /** @brief
        * Default method returning Gray channel for an image.                     \n
		If an image is RGBA or RGB, returns luminosity method gray calculated     \n
        from r g b components. Otherwise - gray channel from 2 channel AlphaGray  \n
		or gray channel values from 1 channel Grayscale.                          \n
        *
        * @return unsigned char * - pointer to grayscale array */

        unsigned char * getGray();

        /** @brief
        * Method returning grayscale channel based on algorithm mode. . \n\n
        * GRAYSCALE_LUMINOSITY - luminosity method                      \n
        * GRAYSCALE_AVERAGE - average mathod                            \n
        *
        * @return unsigned char * - pointer to mask array */

		unsigned char * getGray(GrayscaleMode mode);

        /** @brief
        * Method returning RGB array (continous) for an image. \n\n
        * If an image is RGB, returns
        * If an image is RGBA (4 channels), return index 0             \n
        * If an image is GrayscaleAlpha (2 channels), returns index 0  \n
        *
        * @return unsigned char * - pointer to mask array */

		unsigned char * getRGB();

		unsigned char * getRGBA();

		// Descructor
		~Image();
};

#endif