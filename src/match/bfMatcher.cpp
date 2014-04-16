/*
 * siftMatcher.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */
#include <vector>

#include "bfMatcher.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"

#include "log/logging.hpp"

namespace Match {
GBL::CmRetCode_t BfMatcher::match(const GBL::Descriptor_t& descriptors1, const GBL::Descriptor_t& descriptors2, GBL::MatchCollection_t& good_matches) const {
	LOG_ENTER("descriptor 1 = %p, descriptor 2 = %p, good_matches = %p", &descriptors1, &descriptors2, &good_matches);
	bool crossCheck = true;
	cv::BFMatcher matcher(cv::NORM_L2, crossCheck);
	matcher.match(descriptors1, descriptors2, good_matches);
	LOG_EXIT("GBL::RESULT_SUCCESS");
	return GBL::RESULT_SUCCESS;
}
}
