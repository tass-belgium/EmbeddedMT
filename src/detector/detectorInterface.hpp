#ifndef DETECTORINTERFACE_HPP
#define DETECTORINTERFACE_HPP

#include "cm/global.hpp"
#include "imProc/imageProcInterface.hpp"

namespace Detector {
class DetectorInterface {
public:
	virtual GBL::CmRetCode_t detect(const GBL::Image_t& inputImage, GBL::KeyPointCollection_t& detectedKeypoints) const = 0;
	virtual GBL::CmRetCode_t detect(const GBL::Image_t& inputImage, GBL::KeyPointCollection_t& detectedKeypoints, const GBL::Image_t& background, const ImageProc::ImageProc& imProc) const = 0;
};
}

#endif
