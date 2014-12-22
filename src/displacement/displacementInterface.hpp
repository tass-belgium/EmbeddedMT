/*
 * displacementInterface.hpp
 *
 *  Created on: Apr 9, 2014
 *      Author: cv
 */

#ifndef DISPLACEMENTINTERFACE_HPP_
#define DISPLACEMENTINTERFACE_HPP_

#include "cm/global.hpp"

namespace EmbeddedMT {
	namespace Displacement {
		class DisplacementInterface {
		public:
			virtual ~DisplacementInterface() {};
			virtual GBL::CmRetCode_t calculateDisplacement(const GBL::MatchCollection_t matches, const GBL::KeyPointCollection_t& keypoints1, const GBL::KeyPointCollection_t& keypoints2, GBL::Displacement_t& displacement) const = 0;
		};
	}
}

#endif /* DISPLACEMENTINTERFACE_HPP_ */
