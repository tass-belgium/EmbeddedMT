/*
 * freak.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: Bart Verhagen
 */
#include "freak.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include "log/logging.hpp"

namespace Descriptor {
GBL::CmRetCode_t Freak::describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const {
	LOG_ENTER("image = %p", &image);

	cv::SimpleBlobDetector::Params params;
	params.filterByColor = false;
	params.filterByArea = true;
	params.filterByCircularity = false;
	params.filterByInertia = false;
	params.filterByConvexity = false;
	
	cv::SimpleBlobDetector detector(params);
	detector.detect(image, keypoints);

	LOG_INFO("Number of keypoints found: %d", (uint32_t)  keypoints.size());

	// computing descriptors
	cv::FREAK extractor;
	extractor.compute(image, keypoints, descriptor);
	LOG_INFO("Number of keypoints found: %d", (uint32_t)  keypoints.size());

	LOG_EXIT("GBL::RESULT_SUCCESS");
	return GBL::RESULT_SUCCESS;
}
}
