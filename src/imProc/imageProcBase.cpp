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
}
