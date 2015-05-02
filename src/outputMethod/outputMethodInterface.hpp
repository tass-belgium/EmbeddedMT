/*
 * outputMethodInterface.hpp
 *
 *  Created on: Apr 18, 2014
 *      Author: cv
 */

#ifndef OUTPUTMETHODINTERFACE_HPP_
#define OUTPUTMETHODINTERFACE_HPP_

#include <string>

#include "cm/global.hpp"

namespace EmbeddedMT {
	namespace OutputMethod {
		class OutputMethodStringInterface {
			public:
				virtual ~OutputMethodStringInterface() {};
				virtual GBL::CmRetCode_t write(const std::string& message) = 0;
		};

		class OutputMethodFrameInterface {
			public:
				virtual ~OutputMethodFrameInterface() {};
				virtual GBL::CmRetCode_t write(const GBL::Frame_t& frame) = 0;
		};

		class OutputMethodDisplacementInterface {
			public:
				virtual ~OutputMethodDisplacementInterface() {};
				virtual GBL::CmRetCode_t write(const GBL::Displacement_t& displacement) = 0;
		};
	}
}

#endif /* OUTPUTMETHODINTERFACE_HPP_ */
