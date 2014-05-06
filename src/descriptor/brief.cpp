/*
 * sift.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */
#include <vector>
#include "brief.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace Descriptor {

GBL::CmRetCode_t Brief::describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const {
	// This can be optimized if we drop this particular interface
	LOG_ENTER("image = %p", &image);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;

	cv::SimpleBlobDetector::Params params;
	params.filterByColor = false;
	params.filterByArea = true;
	params.filterByCircularity = false;
	params.filterByInertia = false;
	params.filterByConvexity = false;
	
	cv::SimpleBlobDetector detector(params);
	detector.detect(image, keypoints);

	// Theshold image
	GBL::Image_t binaryImage;
	double thresh = 150;
	cv::threshold(image, binaryImage, thresh, 255, cv::THRESH_BINARY);

	std::vector<std::vector<cv::Point> > contours;
	std::vector<cv::Vec4i> hierarchy;
	
	cv::findContours(binaryImage, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	LOG_INFO("Found %d contours", (uint32_t) contours.size());
	for(size_t i = 0; i < contours.size(); i++) {
		uint32_t x = 0;
		uint32_t y = 0;
		for(size_t j = 0; j < contours[i].size(); j++) {
			x += (contours[i])[j].x;
			y += (contours[i])[j].y;		
		}
		float_t x_center = (float_t) x/ (float_t) contours[i].size();
		float_t y_center = (float_t) y/ (float_t) contours[i].size();
		GBL::KeyPoint_t keypoint(x_center, y_center, 1); 
		keypoints.push_back(keypoint);
	}

	cv::BriefDescriptorExtractor extractor(16);
	extractor.compute(image, keypoints, descriptor);
	result = GBL::RESULT_SUCCESS;

	LOG_EXIT("result = %d", result);
	return result;
}
}
