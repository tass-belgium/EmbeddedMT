/*
 * sift.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */

#ifndef BRIEF_HPP_
#define BRIEF_HPP_

#include "descriptorInterface.hpp"

namespace EmbeddedMT {
	namespace Descriptor {
		class Brief : public DescriptorInterface {
		public:
			virtual GBL::CmRetCode_t describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const;
		};
	}
}
#endif /* BRIEF_HPP_ */
