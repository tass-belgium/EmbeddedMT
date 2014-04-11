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
	virtual GBL::CmRetCode_t describe(const GBL::Image_t image, uint8_t scaleLevels, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const;
};
}

#endif /* SIFT_HPP_ */
