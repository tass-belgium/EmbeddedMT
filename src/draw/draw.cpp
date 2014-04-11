/*
 * draw.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */
#include "draw.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/photo/photo.hpp"

namespace Draw {
GBL::CmRetCode_t Draw::draw(const GBL::Image_t& img1, const GBL::Image_t& img2, std::vector<GBL::Match_t> good_matches,
		std::vector<GBL::KeyPoint_t> keypoints1, std::vector<GBL::KeyPoint_t> keypoints2) const {
	//-- Draw only "good" matches
	GBL::Image_t img_matches;
	cv::drawMatches(img1, keypoints1, img2, keypoints2, good_matches,
			img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1),
			cv::vector<char>(), cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- Show detected matches
	cv::imshow("Good Matches", img_matches);
	return GBL::RESULT_SUCCESS;
}
}
