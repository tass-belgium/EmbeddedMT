#ifndef FINDCONTOURS_HPP
#define FINDCONTOURS_HPP

#include <vector>
#include "cm/global.hpp"
#include "growRegions.hpp"
#include "imProc/imageProcBase.hpp"
#include "imProc/and.hpp"

namespace EmbeddedMT {
	namespace ImageProc {
		class Contours {
			public:
				Contours(const region_t minimumRegionSize = 2, const uint8_t maskWidthOneSide = 1);
				
//				std::vector<std::vector<GBL::Point> > find(const GBL::Image_t& image, const uint8_t maskWidthOneSide);

				template<unsigned maskWidthOneSide>
				std::vector<std::vector<GBL::Point> > find(const GBL::Image_t& image);


			private:
				Contours(void);
				GrowRegions _region; 
		};


		template<unsigned maskWidthOneSide>
		std::vector<std::vector<GBL::Point> > Contours::find(const GBL::Image_t& image) {
			// Assumption: the image was quantized where only one bit is non-zero. The non-zero bit defines to which quantization class the bit belongs
			// Step 1: And bits and all neighbouring bits. 
			GBL::Image_t tmpImage = EmbeddedMT::ImageProc::Meta::AndSimd<uint8_t, GBL::U_REGISTER_SIZE, maskWidthOneSide>::fourNeighbourhood(image);

			// Step 2: Grow uniform regions
			return _region.growUniform(tmpImage);
		}
	}
}
#endif
