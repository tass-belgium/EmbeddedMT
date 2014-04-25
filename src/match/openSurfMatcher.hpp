/*
 * openSurfMatcher.hpp
 *
 *  Created on: Apr 23, 2014
 *      Author: Bart Verhagen
 */

#ifndef OPENSURFMATCHER_HPP_
#define OPENSURFMATCHER_HPP_

#include "matchInterface.hpp"

namespace Match {
class OpenSurfMatcher : public MatcherInterface {
public:
	OpenSurfMatcher(void);
	virtual GBL::CmRetCode_t match(const GBL::Descriptor_t& descriptors1, const GBL::Descriptor_t& descriptors2, GBL::MatchCollection_t& good_matches) const;
private:
	inline float_t subtractFloat(const GBL::Descriptor_t& descriptor1, const uint32_t row1, const GBL::Descriptor_t& descriptor2, const uint32_t row2) const;

	const float_t _threshold;
};
}

#endif /* OPENSURFMATCHER_HPP_ */
