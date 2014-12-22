/*
 * sift.hpp
 *
 *  Created on: Apr 23, 2014
 *      Author: Bart Verhagen
 */

#ifndef ORB_HPP_
#define ORB_HPP_

#include "descriptorInterface.hpp"

namespace EmbeddedMT {
	namespace Descriptor {
		class Orb : public DescriptorInterface {
		public:
			Orb(void);
			virtual GBL::CmRetCode_t describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const;
		private:
			const int32_t _nfeatures;
				const float_t _scaleFactor;
				const int _nlevels;
				const int32_t _edgeThreshold;
				const int32_t _firstLevel;
				const int32_t _WTA_K;
				const int32_t _scoreType;
				const int32_t _patchSize;
		};
	}
}

#endif /* ORB_HPP_ */
