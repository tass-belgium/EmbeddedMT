#include <vector>
#include <iostream>

#include "check.h"

#include "cm/global.hpp"
#include "cm/point.hpp"
#include "cm/utils.hpp"
#include "../growRegions.hpp"

#include "testUtils.hpp"

#include "growRegionsTestSuite.hpp"

using EmbeddedMT::ImageProc::GrowRegions;
using EmbeddedMT::test::Utils;

namespace EmbeddedMT {
	namespace test {
		START_TEST(growRegionsSimple)
		{
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

			GrowRegions growRegions(2, 1, 0x01U);
			GBL::Image_t tmpImage(imageLength, imageLength, CV_8UC1, testImage);
			std::vector<std::vector<GBL::Point> > regions = growRegions.growUniform(tmpImage);

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
			ck_assert_int_eq(regions.size(), 1);
			ck_assert_int_eq(Utils::contourPointsMatch(regions, rightImage, imageLength, imageLength), true);
		}
		END_TEST

		START_TEST(growRegionsSimple2)
		{
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

			GrowRegions growRegions(2, 1, 0x01U);
			GBL::Image_t tmpImage(imageLength, imageLength, CV_8UC1, testImage);
			std::vector<std::vector<GBL::Point> > regions = growRegions.growUniform(tmpImage);

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
			ck_assert_int_eq(regions.size(), 2);
			ck_assert_int_eq(Utils::contourPointsMatch(regions, rightImage, imageLength, imageLength), true);
		}
		END_TEST

		START_TEST(growRegionsSimple3)
		{
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

			GrowRegions growRegions(1, 1, 0x01U);
			GBL::Image_t tmpImage(imageLength, imageLength, CV_8UC1, testImage);
			std::vector<std::vector<GBL::Point> > regions = growRegions.growUniform(tmpImage);

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
			ck_assert_int_eq(regions.size(), 4);
			ck_assert_int_eq(Utils::contourPointsMatch(regions, rightImage, imageLength, imageLength), true);
		}
		END_TEST

		START_TEST(growRegionsSimple4)
		{
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

			GrowRegions growRegions(1, 1, 0x0U);
			GBL::Image_t tmpImage(imageLength, imageLength, CV_8UC1, testImage);
			std::vector<std::vector<GBL::Point> > regions = growRegions.growUniform(tmpImage);

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
			ck_assert_int_eq(regions.size(), 4);
			ck_assert_int_eq(Utils::contourPointsMatch(regions, rightImage, imageLength, imageLength), true);
		}
		END_TEST
		
		START_TEST(courserRegion1)
		{
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

			GrowRegions growRegions(2, 1, 0x0U);
			GBL::Image_t tmpImage(imageLength, imageLength, CV_8UC1, testImage);
			std::vector<std::vector<GBL::Point> > regions = growRegions.growUniform(tmpImage);

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
			ck_assert_int_eq(regions.size(), 1);
			ck_assert_int_eq(Utils::contourPointsMatch(regions, rightImage, imageLength, imageLength), true);
		}
		END_TEST
		Suite * growRegionsTestSuite(void)
		{
			Suite *s;
			TCase *tc_core;

			s = suite_create("Grow regions tests");

			tc_core = tcase_create("Simple test");
			tcase_add_test(tc_core, growRegionsSimple);
			suite_add_tcase(s, tc_core);
		
			tc_core = tcase_create("Simple test 2");
			tcase_add_test(tc_core, growRegionsSimple2);
			suite_add_tcase(s, tc_core);

			tc_core = tcase_create("Simple test 3");
			tcase_add_test(tc_core, growRegionsSimple3);
			suite_add_tcase(s, tc_core);
			
			tc_core = tcase_create("Simple test 4");
			tcase_add_test(tc_core, growRegionsSimple4);
			suite_add_tcase(s, tc_core);
			
			tc_core = tcase_create("Courser region 1");
			tcase_add_test(tc_core, courserRegion1);
			suite_add_tcase(s, tc_core);

			return s;
		}
	}
}
