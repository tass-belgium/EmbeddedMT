/*
 * displacementBase.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: cv
 */
#include "displacementBase.hpp"

namespace Displacement {
GBL::CmRetCode_t DisplacementBase::calculateDisplacement(GBL::MatchCollection_t matches, GBL::KeyPointCollection_t& keypoints1, GBL::KeyPointCollection_t& keypoints2, GBL::Displacement_t& displacement) const {
	float_t x = 0;
	float_t y = 0;
	for(uint32_t i = 0; i < matches.size(); i++) {
		GBL::KeyPoint_t keypoint1 = keypoints1[matches[i].queryIdx];
		GBL::KeyPoint_t keypoint2 = keypoints2[matches[i].trainIdx];

		x += keypoint2.pt.x - keypoint1.pt.x;
		y += keypoint2.pt.y - keypoint1.pt.y;
	}
	displacement.x = x/(float_t) matches.size();
	displacement.y = y/(float_t) matches.size();
	return GBL::RESULT_SUCCESS;
}
}
