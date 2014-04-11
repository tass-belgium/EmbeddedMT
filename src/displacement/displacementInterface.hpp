/*
 * displacementInterface.hpp
 *
 *  Created on: Apr 9, 2014
 *      Author: cv
 */

#ifndef DISPLACEMENTINTERFACE_HPP_
#define DISPLACEMENTINTERFACE_HPP_

#include "cm/global.hpp"

namespace Displacement {
class DisplacementInterface {
public:
	virtual GBL::CmRetCode_t calculateDisplacement(GBL::MatchCollection_t matches, GBL::KeyPointCollection_t& keypoints1, GBL::KeyPointCollection_t& keypoints2, GBL::Displacement_t& displacement) const = 0;
};
}

#endif /* DISPLACEMENTINTERFACE_HPP_ */
