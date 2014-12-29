#include <vector>
#include <iostream>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "cm/global.hpp"
#include "cm/point.hpp"
#include "cm/utils.hpp"
#include "testUtils/testUtils.hpp"

#include "../growRegions.hpp"

using EmbeddedMT::ImageProc::GrowRegions;
using EmbeddedMT::test::Utils;

namespace EmbeddedMT {
	namespace test {
		SCENARIO("Simple test for growing 1 region", "[growRegions]")
		{
			GIVEN("A test image and its correct result") {
				const uint32_t imageLength = 10;
				uint8_t testImage[imageLength*imageLength] = {    			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
																			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
																			1, 1, 8, 8, 8, 8, 8, 8, 1, 1,
																			1, 1, 8, 8, 8, 8, 8, 8, 1, 1,
																			1, 1, 8, 8, 8, 8, 8, 8, 1, 1,
																			1, 1, 8, 8, 8, 8, 8, 8, 1, 1,
																			1, 1, 8, 8, 8, 8, 8, 8, 1, 1,
																			1, 1, 8, 8, 8, 8, 8, 8, 1, 1,
																			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
																			1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

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
				const uint8_t correctNbOfContours = 1U;

				WHEN("We grow regions on the test image") {
					GrowRegions growRegions(2, 1, 0x01U);
					GBL::Image_t tmpImage(imageLength, imageLength, CV_8UC1, testImage);
					std::vector<std::vector<GBL::Point> > regions = growRegions.growUniform(tmpImage);
					
					THEN("We should get the same result as the correct result") {
						REQUIRE(regions.size() == correctNbOfContours);
						REQUIRE(Utils::contourPointsMatch(regions, rightImage, imageLength, imageLength) == true);
					}
				}
			}
		}

		SCENARIO("Simple test for growing 2 regions", "[growRegions]")
		{
			GIVEN("A test image and its correct result") {
				const uint32_t imageLength = 10;
				uint8_t testImage[imageLength*imageLength] = {    1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
																			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
																			1, 1, 8, 8, 1, 1, 8, 8, 1, 1,
																			1, 1, 8, 8, 1, 1, 8, 8, 1, 1,
																			1, 1, 8, 8, 1, 1, 8, 8, 1, 1,
																			1, 1, 8, 8, 1, 1, 8, 8, 1, 1,
																			1, 1, 8, 8, 1, 1, 8, 8, 1, 1,
																			1, 1, 8, 8, 1, 1, 8, 8, 1, 1,
																			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
																			1, 1, 1, 1, 1, 1, 1, 1, 1, 1};


				// Implementation specific: this particular method will remove shifted edges
				const uint8_t rightImage[imageLength*imageLength] = {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																		0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
																		0, 1, 0, 0, 1, 1, 0, 0, 1, 0,
																		0, 1, 0, 0, 1, 1, 0, 0, 1, 0,
																		0, 1, 0, 0, 1, 1, 0, 0, 1, 0,
																		0, 1, 0, 0, 1, 1, 0, 0, 1, 0,
																		0, 1, 0, 0, 1, 1, 0, 0, 1, 0,
																		0, 1, 0, 0, 1, 1, 0, 0, 1, 0,
																		0, 0, 1, 1, 0, 0, 1, 1, 0, 0,
																		0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
				const uint8_t correctNbOfContours = 2U;

				WHEN("We grow regions on the test image") {
					GrowRegions growRegions(2, 1, 0x01U);
					GBL::Image_t tmpImage(imageLength, imageLength, CV_8UC1, testImage);
					std::vector<std::vector<GBL::Point> > regions = growRegions.growUniform(tmpImage);

					THEN("We should obtain the same result as the correct result") {
						REQUIRE(regions.size() == 2);
						REQUIRE(Utils::contourPointsMatch(regions, rightImage, imageLength, imageLength) == true);
					}
				}
			}
		}
		
		SCENARIO("Simple test for growing 4 regions", "[growRegions]")
		{
			GIVEN("A test image and its correct result") {
				const uint32_t imageLength = 10;
				uint8_t testImage[imageLength*imageLength] = {    			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
																			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
																			1, 1, 8, 1, 1, 1, 8, 8, 1, 1,
																			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
																			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
																			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
																			1, 1, 8, 8, 1, 1, 1, 8, 1, 1,
																			1, 1, 8, 8, 1, 1, 1, 8, 1, 1,
																			1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
																			1, 1, 1, 1, 1, 1, 1, 1, 1, 1};


				// Implementation specific: this particular method will remove shifted edges
				const uint8_t rightImage[imageLength*imageLength] = {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																		0, 0, 1, 0, 0, 0, 1, 1, 0, 0,
																		0, 1, 0, 1, 0, 1, 0, 0, 1, 0,
																		0, 0, 1, 0, 0, 0, 1, 1, 0, 0,
																		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																		0, 0, 1, 1, 0, 0, 0, 1, 0, 0,
																		0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
																		0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
																		0, 0, 1, 1, 0, 0, 0, 1, 0, 0,
																		0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
				const uint8_t correctNbOfContours = 4U;

				WHEN("We grow regions on the test image") {
					GrowRegions growRegions(1, 1, 0x01U);
					GBL::Image_t tmpImage(imageLength, imageLength, CV_8UC1, testImage);
					std::vector<std::vector<GBL::Point> > regions = growRegions.growUniform(tmpImage);

					THEN("We obtain the same result as described by the correct result") {
						REQUIRE(regions.size() == correctNbOfContours);
						REQUIRE(Utils::contourPointsMatch(regions, rightImage, imageLength, imageLength) == true);
					}
				}
			}
		}

	 	SCENARIO("Simple test for growing 4 other regions", "[growRegions]")
		{
			GIVEN("A test image and its correct result") {
				const uint32_t imageLength = 10;
				uint8_t testImage[imageLength*imageLength] = {    			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																			0, 0, 8, 0, 0, 0, 8, 8, 0, 0,
																			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																			0, 0, 8, 8, 0, 0, 0, 8, 0, 0,
																			0, 0, 8, 8, 0, 0, 0, 8, 0, 0,
																			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																			0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


				// Implementation specific: this particular method will remove shifted edges
				const uint8_t rightImage[imageLength*imageLength] = {  0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																		0, 0, 1, 0, 0, 0, 1, 1, 0, 0,
																		0, 1, 0, 1, 0, 1, 0, 0, 1, 0,
																		0, 0, 1, 0, 0, 0, 1, 1, 0, 0,
																		0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																		0, 0, 1, 1, 0, 0, 0, 1, 0, 0,
																		0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
																		0, 1, 0, 0, 1, 0, 1, 0, 1, 0,
																		0, 0, 1, 1, 0, 0, 0, 1, 0, 0,
																		0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
				const uint8_t correctNbOfContours = 4U;

				WHEN("We grow regions on the test image") {
					GrowRegions growRegions(1, 1, 0x0U);
					GBL::Image_t tmpImage(imageLength, imageLength, CV_8UC1, testImage);
					std::vector<std::vector<GBL::Point> > regions = growRegions.growUniform(tmpImage);
					
					THEN("We should obtain the same result as the correct result") {
						REQUIRE(regions.size() == correctNbOfContours);
						REQUIRE(Utils::contourPointsMatch(regions, rightImage, imageLength, imageLength) == true);
					}
				}
			}
		}
		
		SCENARIO("Simple test for growing courser regions", "[growRegions]")
		{
			GIVEN("A test image and its correct result") {
				const uint32_t imageLength = 10;
				uint8_t testImage[imageLength*imageLength] = {    			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																			0, 0, 8, 0, 0, 0, 8, 8, 0, 0,
																			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																			0, 0, 8, 8, 0, 0, 0, 8, 0, 0,
																			0, 0, 8, 8, 0, 0, 0, 8, 0, 0,
																			0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
																			0, 0, 0, 0, 0, 0, 0, 0, 0, 0};


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
				const uint8_t correctNbOfContours = 1U;

				WHEN("We grow regions on the test image") {
					GrowRegions growRegions(2, 1, 0x0U);
					GBL::Image_t tmpImage(imageLength, imageLength, CV_8UC1, testImage);
					std::vector<std::vector<GBL::Point> > regions = growRegions.growUniform(tmpImage);

					THEN("We should obtain the same result as the correct result") {
						REQUIRE(regions.size() == 1);
						REQUIRE(Utils::contourPointsMatch(regions, rightImage, imageLength, imageLength) == true);
					}
				}
			}
		}
	}
}
