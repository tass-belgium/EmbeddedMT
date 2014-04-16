/*
 * sift.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */
#include "surf.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

namespace Descriptor {
GBL::CmRetCode_t Surf::describe(const GBL::Image_t image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const {
	const uint32_t hessianThreshold = 100;
	const uint32_t nOctaves = 4;
	const uint32_t nOctaveLayers=1;
	const bool_t extended=true;
	const bool_t upright=false;

	// detecting keypoints
	cv::SurfFeatureDetector detector(hessianThreshold, nOctaves, nOctaveLayers, extended, upright);

	detector.detect(image, keypoints);

	// computing descriptors
	cv::SurfDescriptorExtractor extractor;
	extractor.compute(image, keypoints, descriptor);
	return GBL::RESULT_SUCCESS;
}
}
