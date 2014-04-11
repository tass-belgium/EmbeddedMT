/*
 * SiftMatcher.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */

#ifndef SIFTMATCHER_HPP_
#define SIFTMATCHER_HPP_

#include "matchInterface.hpp"

namespace Match {
class BfMatcher : public MatcherInterface {
public:
	virtual GBL::CmRetCode_t match(GBL::Descriptor_t& descriptors1, GBL::Descriptor_t& descriptors2, GBL::MatchCollection_t& good_matches) const;
};
}

#endif /* SIFTMATCHER_HPP_ */
