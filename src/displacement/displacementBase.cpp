/*
 * displacementBase.cpp
 *
 *  Created on: Apr 9, 2014
 *      Author: cv
 */

#include "displacementBase.hpp"

#include "log/logging.hpp"

namespace EmbeddedMT {
	namespace Displacement {
		GBL::CmRetCode_t DisplacementBase::calculateDisplacement(const GBL::MatchCollection_t matches, const GBL::KeyPointCollection_t& keypoints1, const GBL::KeyPointCollection_t& keypoints2, GBL::Displacement_t& displacement) const {
			LOG_ENTER("matches = %p", &matches);
			displacement.x = 0;
			displacement.y = 0;
			float_t x = 0;
			float_t y = 0;
			uint32_t nbMovingPoints = 0;
			const float_t threshold = 2.0f;
			for(uint32_t i = 0; i < matches.size(); i++) {
				GBL::KeyPoint_t keypoint1 = keypoints1[matches[i].queryIdx];
				GBL::KeyPoint_t keypoint2 = keypoints2[matches[i].trainIdx];
			
				float_t dis_x = keypoint2.pt.x - keypoint1.pt.x;
				float_t dis_y = keypoint2.pt.y - keypoint1.pt.y;

				if(abs(dis_x) >= threshold || abs(dis_y) >= threshold) {
					x += dis_x;
					y += dis_y;
					nbMovingPoints++;
				}
			}
			if(nbMovingPoints > 0) {
				displacement.x = x/(float_t) nbMovingPoints;
				displacement.y = y/(float_t) nbMovingPoints;
			} else {
				displacement.x = 0;
				displacement.y = 0;
			}
			LOG_INFO("X displacement = %d", displacement.x);
			LOG_INFO("Y displacement = %d", displacement.y);
			LOG_EXIT("GBL::RESULT_SUCCESS");
			return GBL::RESULT_SUCCESS;
		}
	}
}
