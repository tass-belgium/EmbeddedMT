/*
 * inputVideo.cpp
 *
 *  Created on: Apr 18, 2014
 *      Author: cv
 */

#include "inputVideo.hpp"

#include "log/logging.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace InputMethod {
GBL::CmRetCode_t InputVideo::start(const char* inputFile) {
	LOG_ENTER("input file = %s", inputFile);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	_videoFile.open(inputFile);
	if (_videoFile.isOpened()) {  // check if we succeeded
		_nbFrames = (uint32_t) _videoFile.get(CV_CAP_PROP_FRAME_COUNT);
		result = GBL::RESULT_SUCCESS;
	} else {
		LOG_ERROR("Could not open %s", inputFile);
	}
	LOG_EXIT("Result = %d", result);
	return result;
}

uint32_t InputVideo::size(void) const {
	return _nbFrames;
}

GBL::CmRetCode_t InputVideo::getFrame(uint16_t index, GBL::Frame_t& frame) {
	LOG_ENTER("Index = %d, target frame = %p", index, &frame);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	if(index < size()) {
		_videoFile.set(CV_CAP_PROP_POS_FRAMES, (double) index);
		LOG_INFO("Frame position: %f", _videoFile.get(CV_CAP_PROP_POS_FRAMES));
		_videoFile.read(frame);
		cv::cvtColor(frame, frame, CV_BGR2GRAY);
		result = GBL::RESULT_SUCCESS;
	}
	LOG_EXIT("result = %d", result);
	return result;
}

GBL::CmRetCode_t InputVideo::getNextFrame(GBL::Frame_t& nextFrame) {
	LOG_ENTER("Getting next frame, videofile = %p", &_videoFile);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	if(_videoFile.read(nextFrame) == true) {
		result = GBL::RESULT_SUCCESS;
		cv::cvtColor(nextFrame, nextFrame, CV_BGR2GRAY);
	} else {
		LOG_ERROR("Could not read next frame");
	}
	LOG_EXIT("Result = %d", result);
	return result;
}
GBL::CmRetCode_t InputVideo::stop(void) {
	_videoFile.release();
	return GBL::RESULT_SUCCESS;
}
}

