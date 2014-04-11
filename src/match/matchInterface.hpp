/*
 * matchInterface.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */

#ifndef MATCHINTERFACE_HPP_
#define MATCHINTERFACE_HPP_

#include "cm/global.hpp"

namespace Match {
class MatcherInterface {
public:
	virtual GBL::CmRetCode_t match(GBL::Descriptor_t& descriptors1, GBL::Descriptor_t& descriptors2, GBL::MatchCollection_t& good_matches) const = 0;
};
}

#endif /* MATCHINTERFACE_HPP_ */
