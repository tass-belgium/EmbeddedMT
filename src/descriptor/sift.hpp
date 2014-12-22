/*
 * sift.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */

#ifndef SIFT_HPP_
#define SIFT_HPP_

#include "descriptorInterface.hpp"

namespace EmbeddedMT {
	namespace Descriptor {
		class Sift : public DescriptorInterface {
		public:
			virtual GBL::CmRetCode_t describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const;
		};
	}
}

#endif /* SIFT_HPP_ */
