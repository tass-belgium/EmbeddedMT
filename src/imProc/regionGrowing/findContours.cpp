#include <vector>
#include <iostream>

#include "growRegions.hpp"
#include "findContours.hpp"

namespace embeddedMT {
	namespace imageProc {
		Contours::Contours(const region_t minimumRegionSize, const uint8_t maskWidthOneSide, const ImageProc* const imProc) :
			_minimumRegionSize(minimumRegionSize), _maskWidthOneSide(maskWidthOneSide), _imProc(imProc) 
		{
		   ; 
		}

		std::vector<std::vector<GBL::Point> > Contours::find(const GBL::Image_t& image) {
			// Assumption: the image was quantized where only one bit is non-zero. The non-zero bit defines to which quantization class the bit belongs
			// Step 1: And bits and all neighbouring bits. 
			GBL::Image_t tmpImage = _imProc->andWithNeighbouringPixels(image, _maskWidthOneSide);

			// Step 2: Grow uniform regions
			GrowRegions region(_minimumRegionSize, _maskWidthOneSide, 0x00);
			std::vector<std::vector<GBL::Point> > contours = region.growUniform(tmpImage);
			return contours;
		}

	}
}
