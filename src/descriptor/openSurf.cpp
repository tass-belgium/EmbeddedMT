/*
 * sift.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */
#include "openSurf.hpp"

#include "surflib.h"
#include "log/logging.hpp"

namespace Descriptor {

OpenSurf::OpenSurf() : _nOctaves(20), _nOctaveLayers(3), _upright(false), _initSample(2), _threshold(0.0004f) {
	;
}

GBL::CmRetCode_t OpenSurf::describeOpenSurf(IplImage* image, IpVec& ipts) const {
	LOG_ENTER("image = %p", &image);

	surfDetDes(image, ipts, _upright, _nOctaves, _nOctaveLayers, _initSample, _threshold);

	LOG_EXIT("GBL::RESULT_SUCCESS");
	return GBL::RESULT_SUCCESS;
}

GBL::CmRetCode_t OpenSurf::describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const {
	// This can be optimized if we drop this particular interface
	LOG_ENTER("image = %p", &image);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	IpVec ipts;

	IplImage tmp_image = image;
	describeOpenSurf(&tmp_image, ipts);

	uint32_t rows = ipts.size();
	if (rows > 0) {
		uint32_t cols = sizeof(ipts[0].descriptor)/sizeof(float_t);
		LOG_INFO("Nb of descriptors: %d", rows);

		// Convert descriptors to interface
		descriptor.create(rows, cols, CV_32FC1);

		// TODO change to memcpy
		for(uint32_t i = 0; i < rows; i++) {
			// TODO change to memcpy
			for(uint32_t j = 0; j < cols; j++) {
				descriptor.at<float_t>(i,j) = ipts[i].descriptor[j];
			}
			cv::KeyPoint tmp(ipts[i].x, ipts[i].y, 0, ipts[i].orientation, 0, ipts[i].scale, -1);
			LOG_INFO("X keypoint: %f", tmp.pt.x);
			keypoints.push_back(tmp);
		}
		result = GBL::RESULT_SUCCESS;
	} else {
		LOG_WARNING("Ipvec was empty");
		result = GBL::RESULT_SUCCESS;
	}

	LOG_EXIT("result = %d", result);
	return result;
}
}
