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
		class OutputMethodInterface {
		public:
			virtual ~OutputMethodInterface() {};
			virtual GBL::CmRetCode_t open(const char* filename) = 0;
			virtual GBL::CmRetCode_t write(const GBL::Displacement_t& displacement) = 0;
			virtual GBL::CmRetCode_t write(const GBL::Frame_t frame) = 0;
			virtual GBL::CmRetCode_t close() = 0;
		};

		class OutputMethodStringInterface : public OutputMethodInterface {
			public:
				virtual GBL::CmRetCode_t write(const std::string& message) = 0;
		};
	}
}

#endif /* OUTPUTMETHODINTERFACE_HPP_ */
