/*
 * =====================================================================================
 *
 *       Filename:  testMaskSuite.cpp
 *
 *    Description:  Test mask suite
 *
 *        Version:  1.0
 *        Created:  04/22/2014 02:48:15 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Bart Verhagen (bv), bart.verhagen@tass.be
 *   Organization:  TASS
 *
 * =====================================================================================
 */
#include	<stdlib.h>
#include <iostream>

#include "check.h"
#include "../imageProcBase.hpp"
#include "testMaskSuite.hpp"

START_TEST (test_mask_simple)
{
	ImageProc::ImageProcBase imProc;
	GBL::Image_t testImage(8,8,CV_8UC1);
	// Set data to zero
	for(uint16_t i = 0; i < testImage.rows*testImage.cols; i++) {
		testImage.data[i] = 0;
	}
	// Input data
	testImage.data[27] = 0xFF;

	GBL::Image_t mask(testImage.rows, testImage.cols, CV_8UC1);

	// Calculate mask
	imProc.getMask(testImage, mask, 1);

	// Construct correct mask
	GBL::Image_t correctMask(testImage.rows, testImage.cols, CV_8UC1);
	ck_assert_int_eq(mask.rows, correctMask.rows);
	ck_assert_int_eq(mask.cols, correctMask.cols);

	for(uint16_t i = 0; i < correctMask.rows * correctMask.cols; i++) {
		correctMask.data[i] = 0;
	}

	correctMask.data[18] = 1;
	correctMask.data[19] = 1;
	correctMask.data[26] = 1;
	correctMask.data[27] = 1;

	// Check mask
	for(uint16_t i = 0; i < correctMask.rows*correctMask.cols; i++) {
		ck_assert_int_eq(mask.data[i], correctMask.data[i]);
	}
}
END_TEST

START_TEST (test_mask_little_less_simple)
{
	ImageProc::ImageProcBase imProc;
	GBL::Image_t testImage(8,8,CV_8UC1);
	// Set data to zero
	for(uint16_t i = 0; i < testImage.rows*testImage.cols; i++) {
		testImage.data[i] = 0;
	}
	// Input data
	testImage.data[27] = 0xFF;
	testImage.data[63] = 0xFF;

	GBL::Image_t mask(testImage.rows, testImage.cols, CV_8UC1);

	// Calculate mask
	imProc.getMask(testImage, mask, 1);

	// Construct correct mask
	GBL::Image_t correctMask(testImage.rows, testImage.cols, CV_8UC1);
	ck_assert_int_eq(mask.rows, correctMask.rows);
	ck_assert_int_eq(mask.cols, correctMask.cols);

	for(uint16_t i = 0; i < correctMask.rows * correctMask.cols; i++) {
		correctMask.data[i] = 0;
	}

	correctMask.data[18] = 1;
	correctMask.data[19] = 1;
	correctMask.data[26] = 1;
	correctMask.data[27] = 1;

	correctMask.data[54] = 1;
	correctMask.data[55] = 1;
	correctMask.data[62] = 1;
	correctMask.data[63] = 1;

	// Check mask
	for(uint16_t i = 0; i < correctMask.rows*correctMask.cols; i++) {
		ck_assert_int_eq(mask.data[i], correctMask.data[i]);
	}
}
END_TEST


Suite * testMask_suite (void)
{
	Suite *s = suite_create ("TestMask");
	
	TCase *tc_core = tcase_create ("Test_mask_simple");
	tcase_add_test (tc_core, test_mask_simple);
	suite_add_tcase (s, tc_core);
	
	tc_core = tcase_create ("Test_mask_little_less_simple");
	tcase_add_test (tc_core, test_mask_little_less_simple);
	suite_add_tcase (s, tc_core);

	
	return s;
}
