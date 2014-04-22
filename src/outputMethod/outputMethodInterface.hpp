/*
 * outputMethodInterface.hpp
 *
 *  Created on: Apr 18, 2014
 *      Author: cv
 */

#ifndef OUTPUTMETHODINTERFACE_HPP_
#define OUTPUTMETHODINTERFACE_HPP_

#include "cm/global.hpp"

namespace OutputMethod {
class OutputMethodInterface {
public:
	virtual ~OutputMethodInterface() {};
	virtual GBL::CmRetCode_t open(const char* filename) = 0;
	virtual GBL::CmRetCode_t write(const GBL::Frame_t) = 0;
	virtual GBL::CmRetCode_t close() = 0;
};
}



#endif /* OUTPUTMETHODINTERFACE_HPP_ */
