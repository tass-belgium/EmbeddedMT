#include <vector>
#include <iostream>

#include "cm/utils.hpp"

#include "testUtils.hpp"

namespace test {
uint32_t Utils::pointAt(const GBL::Point& point, const uint32_t sizeX, const uint32_t sizeY) {
    return (point.getY() * sizeX + point.getX());
}

void Utils::copyImage(const uint8_t* const srcImage, uint8_t* const destImage, const uint32_t sizeX, const uint32_t sizeY) {
    // TODO can be done more efficiently usign memcpy
    
    // For portability switch to point and call appropriate function
	GBL::Point tmpPoint;
    for(uint32_t i = 0; i < sizeY; ++i) {
        tmpPoint.setY(i);
        for(uint32_t j = 0; j < sizeX; ++j) {
            tmpPoint.setX(j);
            destImage[pointAt(tmpPoint, sizeX, sizeY)] = srcImage[pointAt(tmpPoint, sizeX, sizeY)];
        }
    }
}

bool Utils::contourPointsMatch(const std::vector<std::vector<GBL::Point> >& contourPoints, const uint8_t* const correctImage, const uint32_t sizeX, const uint32_t sizeY) {
    uint8_t tmpImage[sizeX*sizeY];
    copyImage(correctImage, tmpImage, sizeX, sizeY); 
    bool pointsMatch = true;

    for (std::vector<std::vector<GBL::Point> >::const_iterator contour = contourPoints.begin() ; contour != contourPoints.end(); ++contour) {
        for(std::vector<GBL::Point>::const_iterator point = contour->begin(); point != contour->end(); ++point) {
            if( correctImage[pointAt(*point, sizeX, sizeY)] == 0 ) {
                std::cout << "Error: point [" << *point << " is found as a contour point but it is not." << std::endl;
                pointsMatch = false;
            } else {
                // Set number of occurrences of point to 0
                if(tmpImage[pointAt(*point, sizeX, sizeY)] > 0) {       // Should always be true, but just in case to avoid overflows
                    tmpImage[pointAt(*point, sizeX, sizeY)]--;
                }
            }
        }
    }
    // Check our correctImage has nothing but zeros now
	GBL::Point tmpPoint;
    for(uint32_t i = 0; i < sizeY; ++i) {
        tmpPoint.setY(i);
        for(uint32_t j = 0; j < sizeX; ++j) {
            tmpPoint.setX(j);
            if( tmpImage[pointAt(tmpPoint, sizeX, sizeY)] != 0) {
                std::cout << "Contourpoints are missing " << int(tmpImage[pointAt(tmpPoint, sizeX, sizeY)]) << " occurence(s) of point " << tmpPoint << std::endl;
                pointsMatch = false;
            }
        }
    }

    // Draw image if points did not match
    if(! pointsMatch) {
        std::cout << "Found contours: " << std::endl;
        printContour(contourPoints, sizeX, sizeY);
        std::cout << "But result should be: " << std::endl;
        printImage(correctImage, sizeX, sizeY);
    }
    return pointsMatch;
}

void Utils::printContour(const std::vector<std::vector<GBL::Point> >& contours, const uint32_t sizeX, const uint32_t sizeY) {
   uint8_t newImage[sizeX*sizeY+1]; 
   // Nulify image
   for(uint32_t i = 0; i < sizeY; i++) {
        for(uint32_t j = 0; j < sizeX; j++) {
            newImage[i * sizeX + j] = 0;
        }
   }

   uint32_t value = 0;
    for (std::vector<std::vector<GBL::Point > >::const_iterator contour = contours.begin() ; contour != contours.end(); ++contour) {
        value++;

        for(std::vector<GBL::Point>::const_iterator point = contour->begin(); point != contour->end(); ++point) {
            newImage[point->getY() * sizeX + point->getX()] += value;        
        }
    }
    printImage(newImage, sizeX, sizeY);
}

void Utils::printImage(const uint8_t* const image, const uint32_t sizeX, const uint32_t sizeY) {
	uint8_t newImage[sizeX*sizeY+1];
	memcpy(newImage, image, sizeof(uint8_t) * (sizeX*sizeY+1));
	GBL::Image_t tmpImage(sizeY, sizeX, CV_8UC1, newImage);
	::Utils::Utils::printImage(tmpImage);
}
}

