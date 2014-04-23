/*
 * freak.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: Bart Verhagen
 */
#include "brisk.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include "log/logging.hpp"

namespace Descriptor {

Brisk::Brisk() : _threshold(10), _octaves(3), _patternScale(1.0f) {
	;
}
GBL::CmRetCode_t Brisk::describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const {
	LOG_ENTER("image = %p", &image);
	cv::BRISK briskDescriptor(_threshold, _octaves, _patternScale);	
	briskDescriptor(image, cv::Mat(), keypoints, descriptor, false);
	LOG_EXIT("GBL::RESULT_SUCCESS");
	return GBL::RESULT_SUCCESS;
}
}
