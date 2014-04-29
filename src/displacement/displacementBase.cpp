/*
 * displacementBase.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: cv
 */

#include "displacementBase.hpp"

#include "log/logging.hpp"

namespace Displacement {
GBL::CmRetCode_t DisplacementBase::calculateDisplacement(const GBL::MatchCollection_t matches, const GBL::KeyPointCollection_t& keypoints1, const GBL::KeyPointCollection_t& keypoints2, GBL::Displacement_t& displacement) const {
	LOG_ENTER("matches = %p", &matches);
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
	LOG_INFO("X displacement = %d", displacement.x);
	LOG_INFO("Y displacement = %d", displacement.y);
	LOG_EXIT("GBL::RESULT_SUCCESS");
	return GBL::RESULT_SUCCESS;
}

}
