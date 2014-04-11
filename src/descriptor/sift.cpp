/*
 * sift.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */
#include "sift.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/nonfree.hpp"

namespace Descriptor {
GBL::CmRetCode_t Sift::describe(const GBL::Image_t image, uint8_t scaleLevels, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const {
	cv::SiftFeatureDetector* detectortype = new cv::SiftFeatureDetector();
	cv::PyramidAdaptedFeatureDetector detector(detectortype, 1); // levels of image scale
	cv::SiftDescriptorExtractor extractor;
	cv::Mat descriptors1;

	detector.detect(image, keypoints);
	extractor.compute(image, keypoints, descriptor);
	return GBL::RESULT_SUCCESS;
}
}
