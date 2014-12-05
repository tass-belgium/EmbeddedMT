#include <vector>

#include "cm/global.hpp"
#include "check.h"

#include "testUtils.hpp"

#include "findContoursTestSuite.hpp"
#include "../findContours.hpp"

using embeddedMT::imageProc::region_t;
using embeddedMT::imageProc::Contours;

namespace embeddedMT
{
	namespace test 
	{
		START_TEST(testFindContoursSimple)
		{
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
			Contours findContours(minimumRegionSize, maskWidthOneSide);
			std::vector<std::vector<GBL::Point> > contours = findContours.find(tmpImage);

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

			ck_assert_int_eq(contours.size(), 1);
			ck_assert_int_eq(::embeddedMT::test::Utils::contourPointsMatch(contours, rightImage, imageLength, imageLength), true);
		}
		END_TEST

		START_TEST(testFindContoursSimple2)
		{
			region_t minimumRegionSize = 2;
			uint8_t maskWidthOneSide = 1;
			const uint32_t imageLength = 10;
			uint8_t testImage[imageLength*imageLength] = {    	1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
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
			Contours findContours(minimumRegionSize, maskWidthOneSide);
			std::vector<std::vector<GBL::Point> > contours = findContours.find(tmpImage);

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
			ck_assert_int_eq(contours.size(), 1);
			ck_assert_int_eq(::embeddedMT::test::Utils::contourPointsMatch(contours, rightImage, imageLength, imageLength), true);
		}
		END_TEST

		Suite * findContoursTestSuite(void)
		{
			Suite *s;
			TCase *tc_core;

			s = suite_create("Find contours tests");

			tc_core = tcase_create("Simple test");
			tcase_add_test(tc_core, testFindContoursSimple);
			suite_add_tcase(s, tc_core);

			tc_core = tcase_create("Simple test 2");
			tcase_add_test(tc_core, testFindContoursSimple2);
			suite_add_tcase(s, tc_core);

			return s;
		}
	}
}
