#ifndef AVERAGECONTOURDETECTOR_HPP
#define AVERAGECONTOURDETECTOR_HPP

#include "detectorInterface.hpp"

namespace Detector {
class  AverageContourDetector : public DetectorInterface {
public:
	AverageContourDetector(void);
	AverageContourDetector(double_t threshold, uint32_t boundaryLength);
	virtual GBL::CmRetCode_t detect(const GBL::Image_t& inputImage, GBL::KeyPointCollection_t& detectedKeypoints) const;
	virtual GBL::CmRetCode_t detect(const GBL::Image_t& inputImage, GBL::KeyPointCollection_t& detectedKeypoints, const GBL::Image_t& background, const ImageProc::ImageProc& imProc) const;
private:
	const double_t _threshold;
	const uint32_t _boundaryLength;
};
}

#endif
