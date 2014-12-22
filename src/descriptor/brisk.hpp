/*
 * sift.hpp
 *
 *  Created on: Apr 23, 2014
 *      Author: Bart Verhagen
 */

#ifndef BRISK_HPP_
#define BRISK_HPP_

#include "descriptorInterface.hpp"

namespace EmbeddedMT {
	namespace Descriptor {
		class Brisk : public DescriptorInterface {
		public:
			Brisk(void);
			virtual GBL::CmRetCode_t describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const;
		private:
			const int32_t _threshold;
				const int32_t _octaves;
			const float_t _patternScale;
		};
	}
}

#endif /* ORB_HPP_ */
