/*
 * sift.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */

#ifndef BRIEF_HPP_
#define BRIEF_HPP_

#include "cm/global.hpp"
#include "descriptorInterface.hpp"

namespace EmbeddedMT {
	namespace Descriptor {
		class Brief : public DescriptorInterface {
		public:
			virtual GBL::CmRetCode_t describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const;
		private:
			void compute(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptors) const;
		};
	}
}
#endif /* BRIEF_HPP_ */
