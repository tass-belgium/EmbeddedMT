/*
 * imageProcInterface.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */

#ifndef IMAGEPROCINTERFACE_HPP_
#define IMAGEPROCINTERFACE_HPP_

#include "cm/global.hpp"

namespace EmbeddedMT{
	namespace ImageProc {
		class ImageProc {
		public:
			virtual ~ImageProc(void) {};
			virtual GBL::CmRetCode_t denoise(const GBL::InputImage_t inputImage, GBL::OutputImage_t outputImage) const = 0;
			virtual GBL::CmRetCode_t sharpen(const GBL::InputImage_t inputImage, GBL::OutputImage_t outputImage, double_t sigmaX, double_t beta) const = 0;
			virtual GBL::CmRetCode_t subtract(const GBL::Image_t firstImage, const GBL::Image_t secondImage, GBL::Image_t outputImage) const = 0;
			virtual GBL::CmRetCode_t fastSubtract(const GBL::Image_t& firstImage, const GBL::Image_t& secondImage, GBL::Image_t& outputImage) const = 0;
			virtual GBL::Image_t andWithNeighbouringPixels(const GBL::Image_t& image, const uint8_t maskWidthOneSide) const = 0;
		};
	}
}

#endif /* IMAGEPROCINTERFACE_HPP_ */
