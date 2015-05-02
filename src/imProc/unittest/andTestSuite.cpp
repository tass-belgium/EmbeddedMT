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

#include "catch.hpp"

#include "cm/global.hpp"
#include "testUtils/testUtils.hpp"
#include "imProc/and.hpp"

using EmbeddedMT::GBL::Frame_t;
using EmbeddedMT::test::Utils;

typedef EmbeddedMT::ImageProc::And ReferenceAnd;

namespace EmbeddedMT {
	namespace test {
		SCENARIO("Test the simple and the optimized versions of the and four neighbourhood function with a 1 pixel one side mask width", "[ImageProc][and four neighbourhood]") {
			GIVEN("A random test image and its correct result") {
				srand(0);
				const uint32_t imageSize = 1601U;
				const uint8_t maskWidthOneSide = 1U;

				Frame_t testImage(imageSize, imageSize, CV_8UC1);
				for(int32_t row = 0; row < testImage.rows; ++row) {
					for(int32_t col = 0; col < testImage.cols; ++col) {
						testImage.at<uint8_t>(row, col) = rand() % std::numeric_limits<uint8_t>::max();
					}
				}

				Frame_t correctImage = ReferenceAnd::fourNeighbourhood(testImage, maskWidthOneSide);

				WHEN("We apply the vectorized simd version of and with neighbouring pixels with the same type to the test image") {
					Frame_t image = ImageProc::Meta::AndSimd<uint8_t, uint8_t, maskWidthOneSide>::fourNeighbourhood(testImage);

					THEN("We should obtain the same correct result") {
						Utils::compareFramesIgnoreBorder<uint8_t>(image, correctImage, maskWidthOneSide);
					}
				}
				WHEN("We apply the vectorized simd version of and with neighbouring pixels with four times the type to the test image") {
					Frame_t image = ImageProc::Meta::AndSimd<uint8_t, uint32_t, maskWidthOneSide>::fourNeighbourhood(testImage);

					THEN("We should obtain the same correct result") {
						Utils::compareFramesIgnoreBorder<uint8_t>(image, correctImage, maskWidthOneSide);
					}
				}
				WHEN("We apply the vectorized simd version of and with neighbouring pixels with eight times the type to the test image") {
					Frame_t image = ImageProc::Meta::AndSimd<uint8_t, uint64_t, maskWidthOneSide>::fourNeighbourhood(testImage);

					THEN("We should obtain the same correct result") {
						Utils::compareFramesIgnoreBorder<uint8_t>(image, correctImage, maskWidthOneSide);
					}
				}
			}
		}
		
		SCENARIO("Test the simple and the optimized versions of the and four neighbourhood function with a 10 pixel maskWidthOneSide", "[ImageProc][and four neighbourhood]") {
			GIVEN("A random test image and its correct result") {
				srand(0);
				const uint32_t imageSize = 1601U;
				const uint8_t maskWidthOneSide = 10U;

				Frame_t testImage(imageSize, imageSize, CV_8UC1);
				for(int32_t row = 0; row < testImage.rows; ++row) {
					for(int32_t col = 0; col < testImage.cols; ++col) {
						testImage.at<uint8_t>(row, col) = rand() % std::numeric_limits<uint8_t>::max();
					}
				}

				Frame_t correctImage = ReferenceAnd::fourNeighbourhood(testImage, maskWidthOneSide);

				WHEN("We apply the vectorized simd version of and with neighbouring pixels with the same type to the test image") {
					Frame_t image = ImageProc::Meta::AndSimd<uint8_t, uint8_t, maskWidthOneSide>::fourNeighbourhood(testImage);

					THEN("We should obtain the same correct result") {
						Utils::compareFramesIgnoreBorder<uint8_t>(image, correctImage, maskWidthOneSide);
					}
				}
				WHEN("We apply the vectorized simd version of and with neighbouring pixels with four times the type to the test image") {
					Frame_t image = ImageProc::Meta::AndSimd<uint8_t, uint32_t, maskWidthOneSide>::fourNeighbourhood(testImage);

					THEN("We should obtain the same correct result") {
						Utils::compareFramesIgnoreBorder<uint8_t>(image, correctImage, maskWidthOneSide);
					}
				}
				WHEN("We apply the vectorized simd version of and with neighbouring pixels with eight times the type to the test image") {
					Frame_t image = ImageProc::Meta::AndSimd<uint8_t, uint64_t, maskWidthOneSide>::fourNeighbourhood(testImage);

					THEN("We should obtain the same correct result") {
						Utils::compareFramesIgnoreBorder<uint8_t>(image, correctImage, maskWidthOneSide);
					}
				}
			}
		}

		SCENARIO("Test the simple and the optimized versions of the and four neighbourhood function with a 0 pixel one mask side size", "[ImageProc][and four neighbourhood]") {
			GIVEN("A random test image and its correct result") {
				srand(0);
				const uint32_t imageSize = 1601U;
				const uint8_t maskWidthOneSide = 0U;

				Frame_t testImage(imageSize, imageSize, CV_8UC1);
				for(int32_t row = 0; row < testImage.rows; ++row) {
					for(int32_t col = 0; col < testImage.cols; ++col) {
						testImage.at<uint8_t>(row, col) = rand() % std::numeric_limits<uint8_t>::max();
					}
				}

				Frame_t correctImage = ReferenceAnd::fourNeighbourhood(testImage, maskWidthOneSide);

				WHEN("We apply the vectorized simd version of and with neighbouring pixels with the same type to the test image") {
					Frame_t image = ImageProc::Meta::AndSimd<uint8_t, uint8_t, maskWidthOneSide>::fourNeighbourhood(testImage);

					THEN("We should obtain the same correct result") {
						Utils::compareFramesIgnoreBorder<uint8_t>(image, correctImage, maskWidthOneSide);
					}
				}
				WHEN("We apply the vectorized simd version of and with neighbouring pixels with four times the type to the test image") {
					Frame_t image = ImageProc::Meta::AndSimd<uint8_t, uint32_t, maskWidthOneSide>::fourNeighbourhood(testImage);

					THEN("We should obtain the same correct result") {
						Utils::compareFramesIgnoreBorder<uint8_t>(image, correctImage, maskWidthOneSide);
					}
				}
				WHEN("We apply the vectorized simd version of and with neighbouring pixels with eight times the type to the test image") {
					Frame_t image = ImageProc::Meta::AndSimd<uint8_t, uint64_t, maskWidthOneSide>::fourNeighbourhood(testImage);

					THEN("We should obtain the same correct result") {
						Utils::compareFramesIgnoreBorder<uint8_t>(image, correctImage, maskWidthOneSide);
					}
				}
			}
		}

		SCENARIO("Test the simple and the optimized versions of the and four neighbourhood function using a huge image", "[ImageProc][and four neighbourhood][longTest]") {
			GIVEN("A random test image and its correct result") {
				srand(0);
				const uint32_t imageSize = 5001U;
				const uint8_t maskWidthOneSide = 1U;

				Frame_t testImage(imageSize, imageSize, CV_8UC1);
				for(int32_t row = 0; row < testImage.rows; ++row) {
					for(int32_t col = 0; col < testImage.cols; ++col) {
						testImage.at<uint8_t>(row, col) = rand() % std::numeric_limits<uint8_t>::max();
					}
				}

				Frame_t correctImage = ReferenceAnd::fourNeighbourhood(testImage, maskWidthOneSide);

				WHEN("We apply the vectorized simd version of and with neighbouring pixels with the same type to the test image") {
					Frame_t image = ImageProc::Meta::AndSimd<uint8_t, uint8_t, maskWidthOneSide>::fourNeighbourhood(testImage);

					THEN("We should obtain the same correct result") {
						Utils::compareFramesIgnoreBorder<uint8_t>(image, correctImage, maskWidthOneSide);
					}
				}
				WHEN("We apply the vectorized simd version of and with neighbouring pixels with four times the type to the test image") {
					Frame_t image = ImageProc::Meta::AndSimd<uint8_t, uint32_t, maskWidthOneSide>::fourNeighbourhood(testImage);

					THEN("We should obtain the same correct result") {
						Utils::compareFramesIgnoreBorder<uint8_t>(image, correctImage, maskWidthOneSide);
					}
				}
				WHEN("We apply the vectorized simd version of and with neighbouring pixels with eight times the type to the test image") {
					Frame_t image = ImageProc::Meta::AndSimd<uint8_t, uint64_t, maskWidthOneSide>::fourNeighbourhood(testImage);

					THEN("We should obtain the same correct result") {
						Utils::compareFramesIgnoreBorder<uint8_t>(image, correctImage, maskWidthOneSide);
					}
				}
			}
		}
	}
}
