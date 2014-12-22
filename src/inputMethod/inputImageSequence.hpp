/*
 * inputVideo.hpp
 *
 *  Created on: Apr 18, 2014
 *      Author: cv
 */

#ifndef INPUTIMAGESEQUENCE_HPP_
#define INPUTIMAGESEQUENCE_HPP_

#include <string>

#include "inputMethodInterface.hpp"

namespace EmbeddedMT {
	namespace InputMethod {
		class InputImageSequence : public InputMethodInterface {
		public:
			virtual bool isMoreInput(void);
			virtual GBL::CmRetCode_t start(const char* inputFile);
			virtual uint32_t size(void) const;
			virtual GBL::CmRetCode_t getFrame(uint16_t index, GBL::Frame_t& frame);
			virtual GBL::CmRetCode_t getNextFrame(GBL::Frame_t& nextFrame);
			virtual GBL::CmRetCode_t stop(void);
		private:
			GBL::CmRetCode_t setFiles (const char* const dir);
			GBL::CmRetCode_t isValidImage(struct dirent* dirp) const;

			std::vector<std::string> _images;
			uint16_t _imageIndex;
		};
	}
}

#endif /* INPUTIMAGESEQUENCE_HPP_ */
