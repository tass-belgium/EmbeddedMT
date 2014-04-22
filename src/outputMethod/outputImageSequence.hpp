/*
 * outputImageSequence.hpp
 *
 *  Created on: Apr 18, 2014
 *      Author: cv
 */

#ifndef OUTPUTIMAGESEQUENCE_HPP_
#define OUTPUTIMAGESEQUENCE_HPP_

#include "outputMethodInterface.hpp"

namespace OutputMethod {
class OutputImageSequence : public OutputMethodInterface {
public:
	virtual GBL::CmRetCode_t open(const char* filename);
	virtual GBL::CmRetCode_t write(const GBL::Frame_t);
	virtual GBL::CmRetCode_t close();

private:
	uint32_t _imageIndex;
	char _filename[GBL::maxFilenameLength];
};
}

#endif /* OUTPUTIMAGESEQUENCE_HPP_ */
