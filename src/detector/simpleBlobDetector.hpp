#ifndef SIMPLEBLOBDETECTOR_HPP
#define SIMPLEBLOBDETECTOR_HPP

#include "detectorInterface.hpp"

namespace EmbeddedMT {
	namespace Detector {
		class  SimpleBlobDetector : public DetectorInterface {
		public:
			virtual GBL::CmRetCode_t detect(const GBL::Image_t& inputImage, GBL::KeyPointCollection_t& detectedKeypoints) const;
			virtual GBL::CmRetCode_t detect(const GBL::Image_t& inputImage, GBL::KeyPointCollection_t& detectedKeypoints, const GBL::Image_t& background, const ImageProc::ImageProc& imProc) const;
		};
	}
}
#endif
