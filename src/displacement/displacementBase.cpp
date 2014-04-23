/*
 * displacementBase.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: cv
 */

#include <utility>
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
	LOG_EXIT("GBL::RESULT_SUCCESS");
	return GBL::RESULT_SUCCESS;
}

GBL::CmRetCode_t DisplacementBase::calculateDisplacement(const IpPairVec& matches, GBL::Displacement_t& displacement) const {
	float_t x = 0;
	float_t y = 0;
	for(uint32_t i = 0; i < matches.size(); i++) {
		x += std::get<1>(matches[i]).x - std::get<0>(matches[i]).x;
		y += std::get<1>(matches[i]).y - std::get<0>(matches[i]).y;
	}
	displacement.x = (int32_t) x/(float_t) matches.size();
	displacement.y = (int32_t) y/(float_t) matches.size();
	return GBL::RESULT_SUCCESS;
}
}
