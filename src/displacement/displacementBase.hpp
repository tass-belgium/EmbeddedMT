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
	virtual GBL::CmRetCode_t calculateDisplacement(const GBL::MatchCollection_t matches, const GBL::KeyPointCollection_t& keypoints1, const GBL::KeyPointCollection_t& keypoints2, GBL::Displacement_t& displacement) const;
	virtual GBL::CmRetCode_t calculateDisplacement(const IpPairVec& matches, GBL::Displacement_t& displacement) const;
};
}

#endif /* DISPLACEMENT_HPP_ */
