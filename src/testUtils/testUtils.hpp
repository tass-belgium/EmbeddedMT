#ifndef TESTUTILS_HPP
#define TESTUTILS_HPP

#include "catch.hpp"

#include "cm/global.hpp"
#include "cm/point.hpp"

namespace EmbeddedMT {
	namespace test {
		class Utils {
			public:
				static inline uint32_t pointAt(const GBL::Point& point, const uint32_t sizeX, const uint32_t sizeY);
				static void copyImage(const uint8_t* const srcImage, uint8_t* const destImage, const uint32_t sizeX, const uint32_t sizeY);
				static bool contourPointsMatch(const std::vector<std::vector<GBL::Point> >& contourPoints, const uint8_t* const correctImage, const uint32_t sizeX, const uint32_t sizeY);
				static void printContour(const std::vector<std::vector<GBL::Point> >& contours, const uint32_t sizeX, const uint32_t sizeY);
				static void printImage(const uint8_t* const image, const uint32_t sizeX, const uint32_t sizeY);

				template <typename valueType>
				static void compareFrames(const GBL::Frame_t& frame1, const GBL::Frame_t& frame2) {
					REQUIRE(frame1.rows == frame2.rows);
					REQUIRE(frame1.cols == frame2.cols);
					REQUIRE(frame1.dims == frame2.dims);
					REQUIRE(frame1.type() == frame2.type());

					// Let's just use the slow method for the tests
					for (int32_t row = 0; row < frame1.rows; ++row) {
						for(int32_t col = 0; col < frame1.cols; ++col) {
							REQUIRE(frame1.at<valueType>(row, col) == frame2.at<valueType>(row, col));
						}
					}
				}

		};
	}
}

#endif
