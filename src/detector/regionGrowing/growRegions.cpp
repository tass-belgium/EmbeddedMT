#include <vector>
#include <iostream>

#include "growRegions.hpp"

#include "detector/regionGrowing/unittest/testUtils.hpp"

GrowRegions::GrowRegions() : _pixelAlreadyConsidered(0xFF), _minimumRegionSize(1), _granularity(1), _backgroundValue(0) {
    ;
}

GrowRegions::GrowRegions(const region_t minimumRegionSize) : _pixelAlreadyConsidered(0xFF), _minimumRegionSize(minimumRegionSize), _granularity(1), _backgroundValue(0) {
    ;
}

GrowRegions::GrowRegions(const region_t minimumRegionSize, const region_t granularity) : _pixelAlreadyConsidered(0xFF), _minimumRegionSize(minimumRegionSize), _granularity(granularity), _backgroundValue(0) {
    ;
}

GrowRegions::GrowRegions(const region_t minimumRegionSize, const region_t granularity, const uint8_t backgroundValue) : _pixelAlreadyConsidered(0xFF), _minimumRegionSize(minimumRegionSize), _granularity(granularity), _backgroundValue(backgroundValue) {
    ;
}

//int GrowRegions::calculateSize(std::vector<Point>& contourPoints) {
//    return 10;
//}

// This is a simple function that works well, but it will likely blow up the stack...
//void GrowRegions::growSeededUniformRecursive(point_t& startingPoint, uint8_t* const image, const uint32_t sizeX, const uint32_t sizeY, uint32_t* regionSize, std::vector<point_t>& contourPoints, const uint8_t value) {
//    if(image[startingPoint.y * sizeX + startingPoint.x] != _pixelAlreadyConsidered) {
//        if(image[startingPoint.y * sizeX + startingPoint.x] != value) {
//           contourPoints.push_back(startingPoint);
//        } else {
            // Set value to _pixelAlreadyConsidered so we will not count it twice
//            image[startingPoint.y * sizeX + startingPoint.x] = _pixelAlreadyConsidered;

            // Check neighbours
//            point_t neighbour;
//            neighbour.x = startingPoint.x;
//            neighbour.y = startingPoint.y;

            // Check horizontal
//            neighbour.x = startingPoint.x - _granularity;
//            growSeededUniformRecursive(neighbour, image, sizeX, sizeY, regionSize, contourPoints, value);

//            neighbour.x = startingPoint.x + _granularity;
//            growSeededUniformRecursive(neighbour, image, sizeX, sizeY, regionSize, contourPoints, value);

//            neighbour.x = startingPoint.x;

            // Check vertical
//            neighbour.y = startingPoint.y - _granularity;
//            growSeededUniformRecursive(neighbour, image, sizeX, sizeY, regionSize, contourPoints, value);

//            neighbour.y = startingPoint.y + _granularity;
//            growSeededUniformRecursive(neighbour, image, sizeX, sizeY, regionSize, contourPoints, value);
//        }
//   }
//    *regionSize = calculateSize(contourPoints);

//}

uint32_t GrowRegions::growSeededUniform(GBL::Point& seed, uint8_t* const image, const uint32_t sizeX, const uint32_t sizeY, std::vector<GBL::Point>& contourPoints) {
    const uint8_t value = image[seed.getY() * sizeX + seed.getX()];
    uint32_t regionSize = 0;
    std::vector<GBL::Point> stack;
    stack.push_back(seed);

	GBL::Point currentPoint;
    while(stack.size() > 0) {
        // After each iteration, pop latest from the stack
        currentPoint = stack.back();
        stack.pop_back();

        // Ignore if point is outside the image boundaries
        if(currentPoint.getY() < sizeY && currentPoint.getX() < sizeX) {
            if(image[currentPoint.getY() * sizeX + currentPoint.getX()] != _pixelAlreadyConsidered) {
                if(image[currentPoint.getY() * sizeX + currentPoint.getX()] != value) {
                    // This point is a contour point. Add to contourpoints and stop this iteration
                    contourPoints.push_back(currentPoint);
                    regionSize++;
                } else {
                    // This means all neighbours have the same value: add all of them to the stack and mark this one as read
                    regionSize++;   // Add neighbour pixels to the region size
                    image[currentPoint.getY() * sizeX + currentPoint.getX()] = _pixelAlreadyConsidered;
                    
					GBL::Point neighbour;
                    neighbour.setX(currentPoint.getX());  // Assign x for safety, compiler should optimize this call
                    neighbour.setY(currentPoint.getY());

                    // Check horizontal
                    neighbour.setX(currentPoint.getX() - _granularity);    // We can jump with _granularity to speedup, but the resulting contour will be courser (as if it is a scaled image)
                    stack.push_back(neighbour);

                    neighbour.setX(currentPoint.getX() + _granularity);
                    stack.push_back(neighbour);

                    // Check vertical
                    neighbour.setX(currentPoint.getX());

                    neighbour.setY(currentPoint.getY() - _granularity);
                    stack.push_back(neighbour);

                    neighbour.setY(currentPoint.getY() + _granularity);
                    stack.push_back(neighbour);
                }
            } else {  
                // Stop and push the previous one from the stack
            }
        }
    }
    return regionSize;
}

std::vector<std::vector<GBL::Point> > GrowRegions::growUniform(uint8_t* const image, const uint32_t sizeX, const uint32_t sizeY) {
    // Try to grow points at a distance of _minimumRegionSize from each other
    std::vector<std::vector<GBL::Point> > foundRegions;
	GBL::Point growPoint(uint32_t(_minimumRegionSize/2), uint32_t(_minimumRegionSize/2));
    while(growPoint.getY() < sizeY) {
        growPoint.setX(0);
        while(growPoint.getX() < sizeX) {
            // If the growPoint is zero, we ignore it
            if(image[growPoint.getY() * sizeX + growPoint.getX()] != 0 && image[growPoint.getY()*sizeX + growPoint.getX()] != _backgroundValue) {
                std::vector<GBL::Point> newRegion;
                uint32_t regionSize = growSeededUniform(growPoint, image, sizeX, sizeY, newRegion);
                if(regionSize >= uint32_t(_minimumRegionSize*_minimumRegionSize)) { 
                    foundRegions.push_back(newRegion);
					std::vector<std::vector<GBL::Point> > tmpRegion;
                }
            }

            // Jump to next candidate point
            growPoint.incrementX(_minimumRegionSize);
        }
        growPoint.incrementY(_minimumRegionSize);
    }
    return foundRegions;
}
