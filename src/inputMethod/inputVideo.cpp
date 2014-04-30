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
bool InputVideo::isMoreInput(void) {
	return (_imageIndex < size());
}

GBL::CmRetCode_t InputVideo::start(const char* inputFile) {
	LOG_ENTER("input file = %s", inputFile);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	_videoFile.open(inputFile);
	if (_videoFile.isOpened()) {  // check if we succeeded
		_nbFrames = (uint32_t) _videoFile.get(CV_CAP_PROP_FRAME_COUNT);
		LOG_INFO("Reading video");
		_frames = new GBL::Image_t[_nbFrames];
		result = GBL::RESULT_SUCCESS;
		for(uint32_t i = 0; i < _nbFrames; i++) {
			if(_videoFile.read(_frames[i]) != true) {
				LOG_ERROR("Could not read frame %d", i);
				result = GBL::RESULT_FAILURE;
			} else {
				cv::cvtColor(_frames[i], _frames[i], CV_BGR2GRAY);
			}		
		}
	} else {
		LOG_ERROR("Could not open %s", inputFile);
	}
	_imageIndex = 0;	
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
		frame = _frames[index];
		result = GBL::RESULT_SUCCESS;
	}
	LOG_EXIT("result = %d", result);
	return result;
}

GBL::CmRetCode_t InputVideo::getNextFrame(GBL::Frame_t& nextFrame) {
	LOG_ENTER("Getting next frame, videofile = %p", &_videoFile);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
#pragma omp critical
	{
	if(_imageIndex < size()) {
		nextFrame = _frames[_imageIndex];
		_imageIndex++;
		result = GBL::RESULT_SUCCESS;
	} else {
		LOG_WARNING("Could not read next frame");
	}
	}
	LOG_INFO("Frame index = %d, Dims: %d, size: %d x %d", _imageIndex, nextFrame.dims, nextFrame.rows, nextFrame.cols);
	LOG_EXIT("Result = %d", result);
	return result;
}
GBL::CmRetCode_t InputVideo::stop(void) {
	_videoFile.release();
//	delete _frames;
	return GBL::RESULT_SUCCESS;
}
}

