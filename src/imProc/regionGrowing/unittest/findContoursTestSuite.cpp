#include <vector>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "cm/global.hpp"
#include "testUtils/testUtils.hpp"

#include "../findContours.hpp"

using EmbeddedMT::ImageProc::region_t;
using EmbeddedMT::ImageProc::Contours;

namespace EmbeddedMT
{
	namespace test 
	{
		SCENARIO("Find 1 simple contour", "[findContours]")
		{
			GIVEN("A simple test image and its correct result") {
				region_t minimumRegionSize = 2;
				uint8_t maskWidthOneSide = 1;

				const uint32_t imageLength = 10;
				uint8_t testImage[imageLength*imageLength] = {    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
																			1, 8, 8, 8, 8, 8, 8, 8, 8, 1,
																			1, 8, 8, 8, 8, 8, 8, 8, 8, 1,
																			1, 8, 8, 8, 8, 8, 8, 8, 8, 1,
																			1, 8, 8, 8, 8, 8, 8, 8, 8, 1,
																			1, 8, 8, 8, 8, 8, 8, 8, 8, 1,
																			1, 8, 8, 8, 8, 8, 8, 8, 8, 1,
																			1, 8, 8, 8, 8, 8, 8, 8, 8, 1,
																			1, 8, 8, 8, 8, 8, 8, 8, 8, 1,
																			1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
				GBL::Image_t tmpImage(imageLength, imageLength, CV_8UC1, testImage);

				// Implementation specific: this particular method will remove shifted edges
				const uint8_t rightImage[imageLength*imageLength] = {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																	0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
																	0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
																	0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
																	0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
																	0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
																	0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
																	0, 1, 0, 0, 0, 0, 0, 0, 1, 0,
																	0, 0, 1, 1, 1, 1, 1, 1, 0, 0,
																	0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
				const uint8_t correctNumberOfContours = 1U;

				WHEN("We apply the findContours() method on the testImage") {
					Contours findContours(minimumRegionSize, maskWidthOneSide);
					std::vector<std::vector<GBL::Point> > contours = findContours.find(tmpImage);
					
					THEN("We should get the same contours as described in the correct result of the image") {
						REQUIRE(contours.size() == correctNumberOfContours);
						REQUIRE(::EmbeddedMT::test::Utils::contourPointsMatch(contours, rightImage, imageLength, imageLength) == true);
					}
				}
			}
		}

		SCENARIO("Find 2 simple contours", "[findContours]")
		{
			GIVEN("A test image and its correct result") {
				region_t minimumRegionSize = 2;
				uint8_t maskWidthOneSide = 1;
				const uint32_t imageLength = 10;
				uint8_t testImage[imageLength*imageLength] = {    			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
																			1, 8, 8, 8, 1, 8, 8, 8, 8, 1,
																			1, 8, 8, 8, 1, 8, 8, 8, 8, 1,
																			1, 8, 8, 8, 1, 8, 8, 8, 8, 1,
																			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
																			1, 8, 8, 8, 8, 1, 8, 8, 8, 1,
																			1, 8, 8, 8, 8, 1, 8, 8, 8, 1,
																			1, 8, 8, 8, 8, 1, 8, 8, 8, 1,
																			1, 8, 8, 8, 8, 1, 8, 8, 8, 1,
																			1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
				GBL::Image_t tmpImage(imageLength, imageLength, CV_8UC1, testImage);

				// Implementation specific: this particular method will remove shifted edges
				const uint8_t rightImage[imageLength*imageLength] = {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																		0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
																		0, 1, 0, 0, 1, 0, 0, 0, 0, 0,
																		0, 1, 0, 0, 1, 0, 0, 0, 0, 0,
																		0, 0, 1, 1, 0, 0, 0, 0, 0, 0,
																		0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
				const uint8_t correctNbOfContours = 1;

				WHEN("We apply the findContours method on the test image") {
					Contours findContours(minimumRegionSize, maskWidthOneSide);
					std::vector<std::vector<GBL::Point> > contours = findContours.find(tmpImage);

					THEN("We must get the same contours as the correct result describes") {
						REQUIRE(contours.size() == correctNbOfContours);
						REQUIRE(::EmbeddedMT::test::Utils::contourPointsMatch(contours, rightImage, imageLength, imageLength) == true);
					}
				}
			}
		}
	}
}
