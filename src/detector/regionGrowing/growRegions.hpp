#ifndef GROW_REGIONS_HPP
#define GROW_REGIONS_HPP

#include <vector>

#include "cm/global.hpp"
#include "cm/point.hpp"

typedef int8_t region_t;

class GrowRegions {
    public:
        GrowRegions();
        GrowRegions(const region_t minimumRegionSize);
        GrowRegions(const region_t minimumRegionSize, const region_t granularity);
        GrowRegions(const region_t minimumRegionSize, const region_t granularity, const uint8_t backgroundValue);

        // Important: this function will DESTROY the image that is given
        std::vector<std::vector<GBL::Point> > growUniform(uint8_t* const image, const uint32_t sizeX, const uint32_t sizeY);

    private:
//        inline int calculateSize(std::vector<Point>& contourPoints);
        inline uint32_t growSeededUniform(GBL::Point& seed, uint8_t* const image, const uint32_t sizeX, const uint32_t sizeY, std::vector<GBL::Point>& contourPoints);

        // Deprecated
//        inline void growSeededUniformRecursive(point_t& startingPoint, uint8_t* const image, const uint32_t sizeX, const uint32_t sizeY, uint32_t* regionSize, std::vector<point_t>& contourPoints, const uint8_t value);

        const uint8_t _pixelAlreadyConsidered;  // Must not be 0 or a value in which only one bit is 1
        const region_t _minimumRegionSize;
        const region_t _granularity;
        const uint8_t _backgroundValue;
};
#endif
