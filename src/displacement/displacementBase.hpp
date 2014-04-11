/*
 * displacement.hpp
 *
 *  Created on: Apr 9, 2014
 *      Author: cv
 */

#ifndef DISPLACEMENTBASE_HPP_
#define DISPLACEMENTBASE_HPP_

#include "displacementInterface.hpp"

namespace Displacement {
class DisplacementBase : public DisplacementInterface {
public:
	virtual GBL::CmRetCode_t calculateDisplacement(GBL::MatchCollection_t matches, GBL::KeyPointCollection_t& keypoints1, GBL::KeyPointCollection_t& keypoints2, GBL::Displacement_t& displacement) const;
};
}

#endif /* DISPLACEMENT_HPP_ */
