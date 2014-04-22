/*
 * sift.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */

#ifndef SURF_HPP_
#define SURF_HPP_

#include "descriptorInterface.hpp"

namespace Descriptor {
class Surf : public DescriptorInterface {
public:
	virtual GBL::CmRetCode_t describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const;
	virtual GBL::CmRetCode_t describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor, const GBL::Image_t& mask) const;
};
}

#endif /* SIFT_HPP_ */
