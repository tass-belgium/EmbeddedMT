/*
 * cmImageProc.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */

#ifndef CMIMAGEPROC_HPP_
#define CMIMAGEPROC_HPP_

#include "imageProcInterface.hpp"

namespace embeddedMT {
	namespace imageProc {
		class ImageProcBase : public ImageProc {
		public:
			virtual ~ImageProcBase(void){};
			virtual GBL::CmRetCode_t denoise(const GBL::InputImage_t inputImage, GBL::OutputImage_t outputImage) const;
			virtual GBL::CmRetCode_t sharpen(const GBL::InputImage_t inputImage, GBL::OutputImage_t outputImage, double_t sigmaX, double_t beta) const;
			virtual GBL::CmRetCode_t subtract(const GBL::Image_t firstImage, const GBL::Image_t secondImage, GBL::Image_t outputImage) const;
			virtual GBL::CmRetCode_t fastSubtract(const GBL::Image_t& firstImage, const GBL::Image_t& secondImage, GBL::Image_t& outputImage) const;
			virtual GBL::Image_t andWithNeighbouringPixels(const GBL::Image_t& image, const uint8_t maskWidthOneSide) const;
		};
	}
}

#endif /* CMIMAGEPROC_HPP_ */
