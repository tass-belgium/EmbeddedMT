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

namespace Match {
GBL::CmRetCode_t BfMatcher::match(GBL::Descriptor_t& descriptors1, GBL::Descriptor_t& descriptors2, GBL::MatchCollection_t& good_matches) const {
	bool crossCheck = true;
	cv::BFMatcher matcher(cv::NORM_L2, crossCheck);
	matcher.match(descriptors1, descriptors2, good_matches);
	return GBL::RESULT_SUCCESS;
}
}
