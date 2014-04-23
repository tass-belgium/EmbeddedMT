/*
 * freak.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: Bart Verhagen
 */
#include "orb.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

#include "log/logging.hpp"

namespace Descriptor {

Orb::Orb() : _nfeatures(10), _scaleFactor(1.1f), _nlevels(60), _edgeThreshold(3), _firstLevel(0), _WTA_K(2), _scoreType(cv::ORB::HARRIS_SCORE), _patchSize(3) {
	;
}
GBL::CmRetCode_t Orb::describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const {
	LOG_ENTER("image = %p", &image);
	cv::ORB orbDescriptor(_nfeatures, _scaleFactor, _nlevels, _edgeThreshold, _firstLevel, _WTA_K, _scoreType, _patchSize);	
	orbDescriptor(image, cv::Mat(), keypoints, descriptor, false);
	LOG_EXIT("GBL::RESULT_SUCCESS");
	return GBL::RESULT_SUCCESS;
}
}
