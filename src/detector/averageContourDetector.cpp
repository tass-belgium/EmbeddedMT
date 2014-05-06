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

#include "averageContourDetector.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace Detector {
AverageContourDetector::AverageContourDetector() : _threshold(20.0), _boundaryLength(30U) {
	;
}

AverageContourDetector::AverageContourDetector(double_t threshold, uint32_t boundaryLength) : _threshold(threshold), _boundaryLength(boundaryLength) {
	;
}

GBL::CmRetCode_t AverageContourDetector::detect(const GBL::Image_t& inputImage, GBL::KeyPointCollection_t& detectedKeypoints) const {
	LOG_ENTER("Image = %p, keypoints = %p", &inputImage, &detectedKeypoints);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	
	cv::findContours(inputImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	LOG_INFO("Found %d contours", (uint32_t) contours.size());
	for(size_t i = 0; i < contours.size(); i++) {
		// Check for the size of the area
		if(contours[i].size() > _boundaryLength) {
			uint32_t x = 0;
			uint32_t y = 0;
			for(size_t j = 0; j < contours[i].size(); j++) {
				x += (contours[i])[j].x;
				y += (contours[i])[j].y;		
			}
			float_t x_center = (float_t) x/ (float_t) contours[i].size();
			float_t y_center = (float_t) y/ (float_t) contours[i].size();
			// TODO: determine scale and angle for more robust keypoints descriptions
			GBL::KeyPoint_t keypoint(x_center, y_center, 1); 
			detectedKeypoints.push_back(keypoint);
		}
	}
	result = GBL::RESULT_SUCCESS;
	LOG_EXIT("result = %d", result);
	return result;
}

GBL::CmRetCode_t AverageContourDetector::detect(const GBL::Image_t& inputImage, GBL::KeyPointCollection_t& detectedKeypoints, const GBL::Image_t& background, const ImageProc::ImageProc& imProc) const {
	GBL::Image_t outputImage = inputImage - background;

	// TODO: make a symmetrical one for darker blobs. This comes with a couple of additional problems...
	cv::threshold(outputImage, outputImage, _threshold, 255, cv::THRESH_BINARY);
	return detect(outputImage, detectedKeypoints);
}
}
