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
#include <iostream>

#include "opencv2/core/core.hpp"
#include "ipoint.h"

#include "cm/global.hpp"
#include "check.h"
#include "../openSurfMatcher.hpp"
#include "testOpenSurfMatcher.hpp"

START_TEST (test_open_surf_matcher_simple)
{
	Match::OpenSurfMatcher matcher;
	GBL::Descriptor_t testDescriptor1(2,5,CV_32FC1);
	// Fill descriptor
	for(int32_t i = 0; i < testDescriptor1.rows; i++) {
		for(int32_t j = 0; j < testDescriptor1.cols; j++) {
			testDescriptor1.at<float>(i, j) = (float_t) (i + j);
		}
	}

	// Perform match
	GBL::MatchCollection_t foundMatches;
	matcher.match(testDescriptor1, testDescriptor1, foundMatches);

	// Perform a manual check
	ck_assert_int_eq(testDescriptor1.rows, foundMatches.size());
	for(uint32_t i = 0; i < foundMatches.size(); i++) {
		ck_assert_int_eq(foundMatches[i].queryIdx,foundMatches[i].trainIdx);
	}
}

END_TEST

Suite * testOpenSurfMatcher_suite (void)
{
	Suite *s = suite_create ("Test openSurf Matcher");
	
	TCase *tc_core = tcase_create ("Test openSurf matcher simple");
	tcase_add_test (tc_core, test_open_surf_matcher_simple);
	suite_add_tcase (s, tc_core);
	
	return s;
}
