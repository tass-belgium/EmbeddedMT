#ifndef TESTUTILS_HPP
#define TESTUTILS_HPP

#include "cm/global.hpp"
#include "cm/point.hpp"

namespace embeddedMT {
	namespace test {
		class Utils {
			public:
				static inline uint32_t pointAt(const GBL::Point& point, const uint32_t sizeX, const uint32_t sizeY);
				static void copyImage(const uint8_t* const srcImage, uint8_t* const destImage, const uint32_t sizeX, const uint32_t sizeY);
				static bool contourPointsMatch(const std::vector<std::vector<GBL::Point> >& contourPoints, const uint8_t* const correctImage, const uint32_t sizeX, const uint32_t sizeY);
				static void printContour(const std::vector<std::vector<GBL::Point> >& contours, const uint32_t sizeX, const uint32_t sizeY);
				static void printImage(const uint8_t* const image, const uint32_t sizeX, const uint32_t sizeY);

		};
	}
}

#endif
