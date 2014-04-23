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
GBL::CmRetCode_t Freak::describe(const GBL::Image_t image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const {
	LOG_ENTER("image = %p", &image);

	const uint32_t hessianThreshold = 100;
	const uint32_t nOctaves = 20;
	const uint32_t nOctaveLayers=3;
	const bool_t extended=true;
	const bool_t upright=false;

	// detecting keypoints
	cv::SurfFeatureDetector detector(hessianThreshold, nOctaves, nOctaveLayers, extended, upright);
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
