/*
 * sift.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */

#ifndef OPENSURF_HPP_
#define OPENSURF_HPP_

#include "descriptorInterface.hpp"

namespace Descriptor {
class OpenSurf : public DescriptorInterface {
public:
	OpenSurf(void);
	virtual GBL::CmRetCode_t describe(const GBL::Image_t& image, GBL::KeyPointCollection_t& keypoints, GBL::Descriptor_t& descriptor) const;

private:
	virtual GBL::CmRetCode_t describeOpenSurf(IplImage* image, IpVec& ipts) const;
	
	const uint32_t _nOctaves;
	const uint32_t _nOctaveLayers;
	const bool_t _upright;
	const uint32_t _initSample;
	const float_t _threshold;

};
}

#endif /* OPENSURF_HPP_ */
