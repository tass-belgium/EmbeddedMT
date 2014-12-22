/*
 * inputVideo.hpp
 *
 *  Created on: Apr 18, 2014
 *      Author: Bart Verhagen
 */

#ifndef INPUTVIDEO_HPP_
#define INPUTVIDEO_HPP_

#include "inputMethodInterface.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace EmbeddedMT {
	namespace InputMethod {
		class InputVideo : public InputMethodInterface {
		public:
			virtual bool isMoreInput(void);
			virtual GBL::CmRetCode_t start(const char* inputFile);
			virtual uint32_t size(void) const;
			virtual GBL::CmRetCode_t getFrame(uint16_t index, GBL::Frame_t& frame);
			virtual GBL::CmRetCode_t getNextFrame(GBL::Frame_t& nextFrame);
			virtual GBL::CmRetCode_t stop(void);
		private:
			cv::VideoCapture _videoFile;
			uint32_t _nbFrames;
			GBL::Image_t* _frames;
			uint32_t _imageIndex;
		};
	}
}

#endif /* INPUTVIDEO_HPP_ */
