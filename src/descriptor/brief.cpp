/*
 * sift.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */
#include "brief.hpp"

#include "opencv2/core/core.hpp"

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

	cv::BriefDescriptorExtractor extractor(16);
	extractor.compute(image, keypoints, descriptor);
	result = GBL::RESULT_SUCCESS;

	LOG_EXIT("result = %d", result);
	return result;
}
}
