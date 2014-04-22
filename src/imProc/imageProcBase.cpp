/*
 * cmImageProc.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */

#include <cmath>

#include "imageProcBase.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/photo/photo.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "log/logging.hpp"

namespace ImageProc {

inline int32_t convert2dRowTo1d(int32_t rowIndex, int32_t imageWidth);

GBL::CmRetCode_t ImageProcBase::denoise(const GBL::InputImage_t inputImage, GBL::OutputImage_t outputImage) const {
	cv::fastNlMeansDenoising(inputImage, outputImage);
	return GBL::RESULT_SUCCESS;
}

GBL::CmRetCode_t ImageProcBase::sharpen(const GBL::InputImage_t inputImage, GBL::OutputImage_t outputImage, double_t sigmaX, double_t beta) const {
	cv::GaussianBlur(inputImage, outputImage, cv::Size(0, 0), sigmaX);
	cv::addWeighted(inputImage, sigmaX/2, outputImage, beta, 0, outputImage);
	return GBL::RESULT_SUCCESS;
}

GBL::CmRetCode_t ImageProcBase::subtract(const GBL::Image_t firstImage, const GBL::Image_t secondImage, GBL::Image_t outputImage) const {
	cv::Mat isBackground;
	int8_t threshold = 20;
	isBackground = firstImage - secondImage;
	for(int32_t i = 0; i < isBackground.rows*isBackground.cols; i++) {
		if(std::abs(isBackground.data[i]) < threshold) {
			isBackground.data[i] = 0;
		} else {
			isBackground.data[i] = 1;
		}
	}
	outputImage = firstImage.mul(isBackground);
	return GBL::RESULT_SUCCESS;
}

GBL::CmRetCode_t ImageProcBase::fastSubtract(const GBL::Image_t& firstImage, const GBL::Image_t& secondImage, GBL::Image_t& outputImage) const {
	LOG_ENTER("first image = %p, second image = %p, output image = %p", &firstImage, &secondImage, &outputImage);
	outputImage = firstImage - secondImage;
	// Rework to be within the range of 0 - 255 again
	outputImage = (outputImage+255)/2;
	LOG_EXIT("GBL::RESULT_SUCCESS");
	return GBL::RESULT_SUCCESS;
}

GBL::CmRetCode_t ImageProcBase::fastSubtractAndMask(const GBL::Image_t& firstImage, const GBL::Image_t& secondImage, GBL::Image_t& outputImage, GBL::Image_t& mask, uint32_t threshold) const {
	LOG_ENTER("first image = %p, second image = %p, output image = %p", &firstImage, &secondImage, &outputImage);
	outputImage = firstImage - secondImage;
	// Calculate mask based on this image
	getMask(outputImage, mask, threshold);	
	// Rework to be within the range of 0 - 255 again
	outputImage = (outputImage+255)/2;
	LOG_EXIT("GBL::RESULT_SUCCESS");
	return GBL::RESULT_SUCCESS;
}

GBL::CmRetCode_t ImageProcBase::getMask(const GBL::Image_t& image, GBL::Image_t& mask, uint32_t threshold) const {
	LOG_ENTER("Image = %p, mask = %p, threshold = %d", &image, &mask, threshold);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	// Calculate integral image
	GBL::Image_t absImage = abs(image);
	GBL::Image_t integralImage;
	cv::integral(absImage, integralImage);
	uint16_t regionHeight = image.rows/4;
	uint16_t regionWidth = image.cols/4;
	LOG_INFO("Total pixels of the image: %d", image.rows*image.cols);
	LOG_INFO("Region width = %d, region height = %d", regionWidth, regionHeight);
	for(uint8_t i = 0; i < 4; i++) {
		LOG_INFO("Region height %d: %d (%d) x %d (%d)", i,convert2dRowTo1d(i*regionHeight, image.cols), i * regionHeight, convert2dRowTo1d((i+1)*regionHeight-1, image.cols), (i+1)* regionHeight-1);
		for(uint8_t j = 0; j < 4; j++) {
			LOG_INFO("Region width %d: %d x %d", j, j * regionWidth, (j+1) * regionWidth-1);
			uint32_t sumOfPixelsInRegion = integralImage.data[convert2dRowTo1d(i+1*regionHeight-1,image.cols) + (j+1) * regionWidth - 1] -
				integralImage.data[convert2dRowTo1d(i*regionHeight,image.cols) + j * regionWidth];

			LOG_INFO("Sum of pixels in region: %d", abs(sumOfPixelsInRegion));
			if((uint32_t) abs(sumOfPixelsInRegion) > threshold) {
				LOG_INFO("Region %d x %d is above threshold", i,j);
				// Fill region
				uint32_t pixelCount = 0;
				for(int32_t k = i * regionHeight; k < (i+1) * regionHeight; k++) {
					int32_t row = convert2dRowTo1d(k, image.cols);
					for(int32_t l = j * regionWidth; l < (j+1) * regionWidth; l++) {
						mask.data[row + l] = (uint8_t) 1;
						pixelCount++;
					}
				}
				LOG_INFO("PixelCount = %d", pixelCount);
			}
		}
	}
	
	cv::imshow("masked image", 255*mask);
	cv::waitKey(0);
	LOG_EXIT("result = %d", result);
	return result;
}

int32_t convert2dRowTo1d(int32_t rowIndex, int32_t imageWidth) {
	return rowIndex * imageWidth;
}
}
