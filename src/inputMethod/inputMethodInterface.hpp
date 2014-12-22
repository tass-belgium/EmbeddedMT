/*
 * inputMethodInterface.hpp
 *
 *  Created on: Apr 18, 2014
 *      Author: cv
 */

#ifndef INPUTMETHODINTERFACE_HPP_
#define INPUTMETHODINTERFACE_HPP_

#include "cm/global.hpp"

namespace EmbeddedMT {
	namespace InputMethod {
		class InputMethodInterface {
		public:
			virtual ~InputMethodInterface() {};
			virtual bool isMoreInput(void) = 0;
			virtual GBL::CmRetCode_t start(const char* inputFile) = 0;
			virtual uint32_t size(void) const = 0;
			virtual GBL::CmRetCode_t getFrame(uint16_t index, GBL::Frame_t& frame) = 0;
			virtual GBL::CmRetCode_t getNextFrame(GBL::Frame_t& nextFrame) = 0;
			virtual GBL::CmRetCode_t stop(void) = 0;
		};
	}
}

#endif /* INPUTMETHODINTERFACE_HPP_ */
