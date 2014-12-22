#ifndef FASTAVERAGECONTOURDETECTOR_HPP
#define FASTAVERAGECONTOURDETECTOR_HPP

#include "detectorInterface.hpp"
#include "imProc/regionGrowing/regions.hpp"

using EmbeddedMT::ImageProc::region_t;

namespace EmbeddedMT {
	namespace Detector {
		class  FastAverageContourDetector : public DetectorInterface {
		public:
			FastAverageContourDetector(const region_t minimumRegionSize = 1, const uint8_t maskWidthOneSide = 1);
			virtual GBL::CmRetCode_t detect(const GBL::Image_t& inputImage, GBL::KeyPointCollection_t& detectedKeypoints) const;
			virtual GBL::CmRetCode_t detect(const GBL::Image_t& inputImage, GBL::KeyPointCollection_t& detectedKeypoints, const GBL::Image_t& background, const ImageProc::ImageProc& imProc) const;
		private:
			const region_t _minimumRegionSize;
			const uint8_t _maskWidthOneSide;
		};
	}
}

#endif
