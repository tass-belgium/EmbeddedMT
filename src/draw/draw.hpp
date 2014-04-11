/*
 * draw.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */

#include "drawInterface.hpp"

namespace Draw {
class Draw : public DrawInterface {
public:
	virtual GBL::CmRetCode_t draw(const GBL::Image_t& img1, const GBL::Image_t& img2, std::vector<GBL::Match_t> good_matches,
			std::vector<GBL::KeyPoint_t> keypoints1, std::vector<GBL::KeyPoint_t> keypoints2) const;
};
}
