/*
 * captureVideo.hpp
 *
 *  Created on: Apr 30, 2014
 *      Author: Bart Verhagen
 */

#ifndef CAPTUREVIDEO_HPP_
#define CAPTUREVIDEO_HPP_

#include "inputMethodInterface.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace EmbeddedMT {
	namespace InputMethod {
		class CaptureVideo : public InputMethodInterface {
		public:
			CaptureVideo(uint8_t cameraId);
			virtual bool isMoreInput(void);
			virtual GBL::CmRetCode_t start(const char* inputFile);
			virtual uint32_t size(void) const;
			virtual GBL::CmRetCode_t getFrame(uint16_t index, GBL::Frame_t& frame);
			virtual GBL::CmRetCode_t getNextFrame(GBL::Frame_t& nextFrame);
			virtual GBL::CmRetCode_t stop(void);
		private:
			cv::VideoCapture _videoFile;
			std::vector<GBL::Image_t> _frameBuffer;
			const uint8_t _cameraId;
			uint32_t _nbFrames;
		};
	}
}


#endif /* CAPTUREVIDEO_HPP_ */
