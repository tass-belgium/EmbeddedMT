/*
 * draw.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */
#include <string>
#include "draw.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/nonfree/nonfree.hpp"
#include "opencv2/photo/photo.hpp"

#include "log/logging.hpp"

namespace Draw {
GBL::CmRetCode_t Draw::draw(const GBL::Image_t& img1, const GBL::Image_t& img2, std::vector<GBL::Match_t> good_matches, std::vector<GBL::KeyPoint_t> keypoints1,
		std::vector<GBL::KeyPoint_t> keypoints2) const {
	//-- Draw only "good" matches
	GBL::Image_t img_matches;
	cv::drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), cv::vector<char>(),
			cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	//-- Show detected matches
	cv::imshow("Good Matches", img_matches);
	return GBL::RESULT_SUCCESS;
}

GBL::CmRetCode_t Draw::openFile(const char* file, uint16_t width, uint16_t height) {
	LOG_ENTER("File = %s", file);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	cv::Size size(width, height);
	const std::string filename(file);
	_videoFile.open(filename, CV_FOURCC('P','I','M','1'), (double) 30.0f, size, true);
	if (_videoFile.isOpened()) {  // check if we succeeded
		result = GBL::RESULT_SUCCESS;
	} else {
		LOG_ERROR("Could not open %s", file);
	}
	LOG_EXIT("result = %d", result);
	return result;
}

GBL::CmRetCode_t Draw::closeFile(void) {
	return GBL::RESULT_SUCCESS;
}

GBL::CmRetCode_t Draw::drawFrame(const GBL::Image_t& img1, const GBL::Image_t& img2, std::vector<GBL::Match_t> good_matches,
		std::vector<GBL::KeyPoint_t> keypoints1, std::vector<GBL::KeyPoint_t> keypoints2) {
	//-- Draw only "good" matches
	GBL::Image_t img_matches;
	cv::drawMatches(img1, keypoints1, img2, keypoints2, good_matches, img_matches, cv::Scalar::all(-1), cv::Scalar::all(-1), cv::vector<char>(),
			cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

//	cv::imshow("Tmp", img_matches);
//	cv::waitKey(0);

	if(_videoFile.isOpened()) {
		_videoFile.write(img_matches);
	} else {
		LOG_ERROR("Video file is not open");
	}
	return GBL::RESULT_SUCCESS;
}
}
