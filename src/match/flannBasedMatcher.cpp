/*
 * siftMatcher.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */
#include <vector>

#include "flannBasedMatcher.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"

#include "log/logging.hpp"

namespace EmbeddedMT {
	namespace Match {
		GBL::CmRetCode_t FlannBasedMatcher::match(const GBL::Descriptor_t& descriptors1, const GBL::Descriptor_t& descriptors2, GBL::MatchCollection_t& good_matches) const {
			LOG_ENTER("descriptor 1 = %p, descriptor 2 = %p, good_matches = %p", &descriptors1, &descriptors2, &good_matches);
			cv::FlannBasedMatcher matcher;
			std::vector<cv::DMatch> matches;
			matcher.match(descriptors1, descriptors2, matches);
			double max_dist = 0;
			double min_dist = 100;

			//-- Quick calculation of max and min distances between keypoints
			for (int i = 0; i < descriptors1.rows; i++) {
				double dist = matches[i].distance;
				if (dist < min_dist)
					min_dist = dist;
				if (dist > max_dist)
					max_dist = dist;
			}

			//-- Draw only "good" matches (i.e. whose distance is less than 2*min_dist,
			//-- or a small arbitary value ( 0.02 ) in the event that min_dist is very
			//-- small)
			//-- PS.- radiusMatch can also be used here.
			for (int i = 0; i < descriptors1.rows; i++) {
				if (matches[i].distance <= cv::max(2 * min_dist, 0.02)) {
					good_matches.push_back(matches[i]);
				}
			}
			LOG_EXIT("GBL::RESULT_SUCCESS");
			return GBL::RESULT_SUCCESS;
		}
	}
}
