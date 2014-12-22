/*
 * captureVideo.cpp
 *
 *  Created on: Apr 30, 2014
 *      Author: Bart Verhagen
 */

#include "captureVideo.hpp"

#include "log/logging.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace EmbeddedMT {
	namespace InputMethod {
		CaptureVideo::CaptureVideo(uint8_t cameraId) : _cameraId(cameraId) {
			;
		}

		bool CaptureVideo::isMoreInput() {
			return true;
		}

		GBL::CmRetCode_t CaptureVideo::start(const char* inputFile) {
			LOG_ENTER("input file = %s", inputFile);
			GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
			_videoFile.open((uint32_t) _cameraId);
			if (_videoFile.isOpened()) {  // check if we succeeded
				result = GBL::RESULT_SUCCESS;
			} else {
				LOG_ERROR("Could not open %s", inputFile);
			}
			_nbFrames = 0;
			LOG_EXIT("Result = %d", result);
			return result;
		}

		uint32_t CaptureVideo::size(void) const {
			return _nbFrames;
		}

		GBL::CmRetCode_t CaptureVideo::getFrame(uint16_t index, GBL::Frame_t& frame) {
			LOG_ENTER("Index = %d, target frame = %p", index, &frame);
			GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
			if(index < size()) {
				frame = _frameBuffer[index];
				result = GBL::RESULT_SUCCESS;
			}
			LOG_EXIT("result = %d", result);
			return result;
		}

		GBL::CmRetCode_t CaptureVideo::getNextFrame(GBL::Frame_t& nextFrame) {
			LOG_ENTER("Getting next frame, videofile = %p", &_videoFile);
			GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
#pragma omp critical
			{
				if(_videoFile.read(nextFrame)) {
					cv::cvtColor(nextFrame, nextFrame, cv::COLOR_BGR2GRAY);
					if(GBL::drawResults_b || GBL::showStuff_b) {
						_frameBuffer.push_back(nextFrame);
					}
					_nbFrames++;
					result = GBL::RESULT_SUCCESS;
					LOG_INFO("Frame index = %d, Dims: %d, size: %d x %d", _nbFrames, nextFrame.dims, nextFrame.rows, nextFrame.cols);
				} else {
					LOG_WARNING("Could not read next frame");
				}
			}
			LOG_EXIT("Result = %d", result);
			return result;
		}
		GBL::CmRetCode_t CaptureVideo::stop(void) {
			_videoFile.release();
			return GBL::RESULT_SUCCESS;
		}
	}
}
