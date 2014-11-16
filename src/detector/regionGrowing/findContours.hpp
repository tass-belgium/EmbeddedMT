#ifndef FINDCONTOURS_HPP
#define FINDCONTOURS_HPP

#include <vector>
#include "cm/global.hpp"
#include "growRegions.hpp"

class Contours {
    public:
        Contours(const region_t minimumRegionSize = 2, const uint8_t maskWidthOneSide = 1);
        std::vector<std::vector<GBL::Point> > find(const GBL::Image_t& image);


    private:
        Contours(void);

		GBL::Image_t andWithNeighbouringPixels(const GBL::Image_t);

		const region_t _minimumRegionSize;
		const uint8_t _maskWidthOneSide;
};
#endif
