#include <vector>
#include <iostream>

#include "growRegions.hpp"
#include "findContours.hpp"

#include "imProc/and.hpp"

typedef EmbeddedMT::ImageProc::And AndClass;

namespace EmbeddedMT {
	namespace ImageProc {
		Contours::Contours(const region_t minimumRegionSize, const uint8_t maskWidthOneSide) :
			_maskWidthOneSide(maskWidthOneSide), _region(minimumRegionSize, maskWidthOneSide, 0x01U) 
		{
		   ; 
		}

		std::vector<std::vector<GBL::Point> > Contours::find(const GBL::Image_t& image) {
			// Assumption: the image was quantized where only one bit is non-zero. The non-zero bit defines to which quantization class the bit belongs
			// Step 1: And bits and all neighbouring bits. 
			GBL::Image_t tmpImage = AndClass::fourNeighbourhood(image, _maskWidthOneSide);

			// Step 2: Grow uniform regions
			return _region.growUniform(tmpImage);
		}

	}
}
