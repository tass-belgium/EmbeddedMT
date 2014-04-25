/*
 * openSurfMatcher.cpp
 *
 *  Created on: Apr 23, 2014
 *      Author: Bart Verhagen
 *
 *  This is the C++ version for openCV 2.4.8 of the openCV matcher as found in ipoint.h (see 3rdparty folder)
 *  Brute force matching based on Lowe's criterion
 */
#include <cfloat>
#include <climits>
#include "surflib.h"
#include "openSurfMatcher.hpp"

#include "log/logging.hpp"

namespace Match {

OpenSurfMatcher::OpenSurfMatcher() : _threshold(0.65f) {
	;
}

inline float_t OpenSurfMatcher::subtractFloat(const GBL::Descriptor_t& descriptor1, const uint32_t row1, const GBL::Descriptor_t& descriptor2, const uint32_t row2) const {
	//LOG_ENTER("descriptor 1 row = %d, descriptor 2 row = %d", row1, row2);
	// TODO Can we vectorize this?
	float_t result = 0;

	const float* row_ptr1 = descriptor1.ptr<float>(row1);
	const float* row_ptr2 = descriptor2.ptr<float>(row2);
	for(int32_t i = 0; i < descriptor1.cols; i++) {
		// Slow but safe method to do this: 
		// 			result += (descriptor1.at<float_t>(row1, i) - descriptor2.at<float_t>(row2,i)) * (descriptor1.at<float_t>(row1, i) - descriptor2.at<float_t>(row2,i));
		// But we live on the edge and use the fast one:
		result += (row_ptr1[i] - row_ptr2[i]) * (row_ptr1[i] - row_ptr2[i]); 	
	}
	//LOG_EXIT("Squared result = %f", result);
	return result;
}

GBL::CmRetCode_t OpenSurfMatcher::match(const GBL::Descriptor_t& descriptors1, const GBL::Descriptor_t& descriptors2, GBL::MatchCollection_t& good_matches) const {
	LOG_ENTER("descriptor 1 = %p, descriptor 2 = %p, good_matches = %p", &descriptors1, &descriptors2, &good_matches);

	const float_t squaredThreshold = _threshold * _threshold;

	for(int32_t i = 0; i < descriptors1.rows; i++) {
		cv::DMatch tmpMatch;
		float_t d1 = FLT_MAX;
		float_t d2 = FLT_MAX;

		for(int32_t j = 0; j < descriptors2.rows; j++) {
			float_t diff = subtractFloat(descriptors1, i, descriptors2, j);

			if(diff < d1) {
				// This feature is the best match up to now
				d2 = d1;
				d1 = diff;
				tmpMatch.trainIdx = j;
			} else if(diff < d2) {
				// This is the second best match up to now
				d2 = diff;
			}
		}

		// Check best and second best matches ratio (Lowe)
		if(d1/d2 < squaredThreshold) {
			// We have a match!
			LOG_INFO("We have a match: %d with %d, ratio = %f", i, tmpMatch.trainIdx, d1/d2);
			tmpMatch.queryIdx = i;
			// Unclear what imgIdx is: set it to 0
			tmpMatch.imgIdx = 0;
			tmpMatch.distance = d1;
		
			good_matches.push_back(tmpMatch);
		}
	}
	LOG_EXIT("GBL::RESULT_SUCCESS");
	return GBL::RESULT_SUCCESS;
}
}
