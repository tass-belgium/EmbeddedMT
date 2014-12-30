/*
 * =====================================================================================
 *
 *       Filename:  imageProcInterfaceTestSuite.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/29/2014 08:06:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Bart Verhagen (bv), bart.verhagen@tass.be
 *   Organization:  TASS
 *
 * =====================================================================================
 */
#include <random>
#include <limits>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "cm/global.hpp"
#include "testUtils/testUtils.hpp"
#include "imProc/and.hpp"

using EmbeddedMT::GBL::Frame_t;
using EmbeddedMT::test::Utils;

namespace EmbeddedMT {
	namespace test {
		SCENARIO("Test the simple and the optimized versions of the andNeighbouringPixels function", "[ImageProc][andNeighbouringPixels]") {
			GIVEN("A random test image and its correct result") {
				srand(0);
				const uint32_t imageSize = 1600U;
				const uint8_t maskWidthOneSide = 1U;

				Frame_t testImage(imageSize, imageSize, CV_8UC1);
				for(int32_t row = 0; row < testImage.rows; ++row) {
					for(int32_t col = 0; col < testImage.cols; ++col) {
						testImage.at<uint8_t>(row, col) = rand() % std::numeric_limits<uint8_t>::max();
					}
				}
				Frame_t correctImage = ImageProc::andWithNeighbouringPixels(testImage, maskWidthOneSide);

				WHEN("We apply the meta version of and with neighbouring pixels to the test image") {
					Frame_t image = ImageProc::Meta::andWithNeighbouringPixels<maskWidthOneSide>(testImage);

					THEN("We should obtain the same correct result") {
						Utils::compareFrames<uint8_t>(image, correctImage);
					}
				}

				WHEN("We apply the vectorized meta version of and with neighbouring pixels to the test image") {
					Frame_t image = ImageProc::Meta::VectorizedAnd<uint8_t, uint8_t, maskWidthOneSide>::andWithNeighbouringPixels(testImage);

					THEN("We should obtain the same correct result") {
						Utils::compareFrames<uint8_t>(image, correctImage);
					}
				}
			}
		}
	}
}
