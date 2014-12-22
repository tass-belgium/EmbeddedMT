/*
 * sift.hpp
 *
 *  Created on: Apr 23, 2014
 *      Author: Bart Verhagen
 */

#ifndef FREAK_HPP_
#define FREAK_HPP_

#include "descriptorInterface.hpp"

namespace EmbeddedMT {
	namespace Descriptor {
		class Freak : public DescriptorInterface {
		public:
			virtual GBL::CmRetCode_t describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const;
		private:
		};
	}
}

#endif /* FREAK_HPP_ */
