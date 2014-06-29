/*
 * =====================================================================================
 *
 *       Filename:  averageContourDetector.cpp
 *
 *    Description:  Detector that calculates the middlepoint of the detected contours
 *
 *        Version:  1.0
 *        Created:  05/06/2014 11:04:10 AM
 *       Revision:  none
 *
 *         Author:  Bart Verhagen (bv), bart.verhagen@tass.be
 *   Organization:  TASS
 *
 * =====================================================================================
 */

#include "simpleBlobDetector.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"

namespace Detector {
GBL::CmRetCode_t SimpleBlobDetector::detect(const GBL::Image_t& inputImage, GBL::KeyPointCollection_t& detectedKeypoints) const {
	LOG_ENTER("Image = %p, keypoints = %p", &inputImage, &detectedKeypoints);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;

	cv::SimpleBlobDetector::Params params;
	params.filterByColor = false;
	params.filterByArea = true;
	params.filterByCircularity = false;
	params.filterByInertia = false;
	params.filterByConvexity = false;
	
	cv::SimpleBlobDetector detector(params);
	detector.detect(inputImage, detectedKeypoints);

	result = GBL::RESULT_SUCCESS;
	LOG_EXIT("result = %d", result);
	return result;
}

GBL::CmRetCode_t SimpleBlobDetector::detect(const GBL::Image_t& inputImage, GBL::KeyPointCollection_t& detectedKeypoints, const GBL::Image_t& background, const ImageProc::ImageProc& imProc) const {
	GBL::Image_t outputImage;
	imProc.fastSubtract(inputImage, background, outputImage); 
	return detect(outputImage, detectedKeypoints);
}
}
