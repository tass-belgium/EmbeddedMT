/*
 * descriptorInterface.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */

#ifndef DESCRIPTORINTERFACE_HPP_
#define DESCRIPTORINTERFACE_HPP_

#include "cm/global.hpp"

#include "ipoint.h"

namespace Descriptor {
class DescriptorInterface {
public:
	virtual ~DescriptorInterface() {};
	virtual GBL::CmRetCode_t describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const = 0;
	virtual GBL::CmRetCode_t describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor, const GBL::Image_t& mask) const = 0;
};
}

#endif /* DESCRIPTORINTERFACE_HPP_ */
