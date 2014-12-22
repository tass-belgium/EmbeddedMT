#ifndef GROW_REGIONS_HPP
#define GROW_REGIONS_HPP

#include <vector>

#include "cm/global.hpp"
#include "cm/point.hpp"

#include "regions.hpp"

namespace EmbeddedMT {
	namespace ImageProc {
		class GrowRegions {
			public:
				GrowRegions(const region_t minimumRegionSize = 1, const region_t granularity = 1, const uint8_t backgroundValue = 0);
			
				// Important: this function will DESTROY the image that is given
				std::vector<std::vector<GBL::Point> > growUniform(GBL::Image_t& image);

				// Safer but slower method for growing uniform regions	
				std::vector<std::vector<GBL::Point> > growUniform(const GBL::Image_t& image);

			private:
				inline uint32_t growSeededUniform(GBL::Point& seed, GBL::Image_t& image, std::vector<GBL::Point>& contourPoints);

				// Deprecated
		//        inline void growSeededUniformRecursive(point_t& startingPoint, uint8_t* const image, const uint32_t sizeX, const uint32_t sizeY, uint32_t* regionSize, std::vector<point_t>& contourPoints, const uint8_t value);

				const uint8_t _pixelAlreadyConsidered;  // Must not be 0 or a value in which only one bit is 1
				const region_t _minimumRegionSize;
				const region_t _granularity;
				const uint8_t _backgroundValue;
		};
	}
}
#endif
