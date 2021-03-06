/*
 * SiftMatcher.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */

#ifndef SIFTMATCHER_HPP_
#define SIFTMATCHER_HPP_

#include "matchInterface.hpp"

namespace EmbeddedMT {
	namespace Match {
		class BfMatcher : public MatcherInterface {
		public:
			virtual GBL::CmRetCode_t match(const GBL::Descriptor_t& descriptors1, const GBL::Descriptor_t& descriptors2, GBL::MatchCollection_t& good_matches) const;
		};
	}
}

#endif /* SIFTMATCHER_HPP_ */
