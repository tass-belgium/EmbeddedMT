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

#include "log/logging.hpp"

namespace Descriptor {
GBL::CmRetCode_t Sift::describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const {
	LOG_ENTER("image = %p", &image);
	cv::SiftFeatureDetector* detectortype = new cv::SiftFeatureDetector();
	cv::PyramidAdaptedFeatureDetector detector(detectortype, 1); // levels of image scale
	cv::SiftDescriptorExtractor extractor;
	cv::Mat descriptors1;

	detector.detect(image, keypoints);
	extractor.compute(image, keypoints, descriptor);
	LOG_EXIT("GBL::RESULT_SUCCESS");
	return GBL::RESULT_SUCCESS;
}

GBL::CmRetCode_t Sift::describe(const GBL::Image_t image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor, const GBL::Image_t& mask) const {
	LOG_ENTER("image = %p", &image);
	cv::SiftFeatureDetector* detectortype = new cv::SiftFeatureDetector();
	cv::PyramidAdaptedFeatureDetector detector(detectortype, 1); // levels of image scale
	cv::SiftDescriptorExtractor extractor;
	cv::Mat descriptors1;

	detector.detect(image, keypoints, mask);
	extractor.compute(image, keypoints, descriptor);
	LOG_EXIT("GBL::RESULT_SUCCESS");
	return GBL::RESULT_SUCCESS;
}
}
