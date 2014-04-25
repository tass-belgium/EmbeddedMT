/*
 * =====================================================================================
 *
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
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

#include "opencv2/core/core.hpp"
#include "ipoint.h"

#include "cm/global.hpp"
#include "check.h"
#include "../openSurfMatcher.hpp"
#include "testOpenSurfMatcher.hpp"

const int32_t IPOINT_DESCRIPTOR_SIZE = (int32_t) (sizeof(Ipoint::descriptor)/sizeof(float));

static inline void matchesAreEqual(const GBL::MatchCollection_t matches1, const GBL::MatchCollection_t matches2) {
	ck_assert_int_eq(matches1.size(), matches2.size());
	for(uint32_t i = 0; i < matches1.size(); i++) {
		bool matchHasCorrespondingMatch = false;
		// Check for corresponding correspondence in matches2
		for(uint32_t j = 0; j < matches2.size(); j++) {
			if(matches1[i].queryIdx == matches2[j].queryIdx) {
				if(matches1[i].trainIdx == matches2[j].trainIdx) {
					matchHasCorrespondingMatch = true;
				}
			}
		}
		ck_assert_int_eq(matchHasCorrespondingMatch, true);
	}
}

static inline GBL::CmRetCode_t convertToIpVec(const GBL::Descriptor_t& descriptor, IpVec& ipts) {
	for(int32_t i = 0; i < descriptor.rows; i++) {
		Ipoint ipoint;
		// Use x coordinate for tracking index
		ipoint.x = (float_t) i;

		for(int32_t j = 0; j < descriptor.cols; j++) {
			ipoint.descriptor[j] = descriptor.at<float>(i,j);
		}
		// Fill the rest with zeros
		for(int32_t j = descriptor.cols; j < IPOINT_DESCRIPTOR_SIZE; j++) {
			ipoint.descriptor[j] = 0.0f;
		}
		ipts.push_back(ipoint);
	}
	return GBL::RESULT_SUCCESS;
}

static inline GBL::CmRetCode_t convertToMatchCollection(const IpPairVec& ipMatches, GBL::MatchCollection_t& matches) {
	for(uint32_t i = 0; i < ipMatches.size(); i++) {
		GBL::Match_t tmpMatch;
		tmpMatch.queryIdx = (int32_t) (std::get<0>(ipMatches[i])).x;
		tmpMatch.trainIdx = (int32_t) (std::get<1>(ipMatches[i])).x;
		matches.push_back(tmpMatch);
	}
	return GBL::RESULT_SUCCESS;
}

static inline void compareWithOriginalOpenSurfMatcher(const GBL::Descriptor_t& descriptor1, const GBL::Descriptor_t& descriptor2, const GBL::MatchCollection_t& foundMatches) {
	IpVec ipts1;
	IpVec ipts2;
	ck_assert_int_eq(convertToIpVec(descriptor1, ipts1), GBL::RESULT_SUCCESS);
	ck_assert_int_eq(convertToIpVec(descriptor2, ipts2), GBL::RESULT_SUCCESS);
	IpPairVec originalMatches;
	
	// Measure time
	clock_t tStart = clock();
	getMatches(ipts1, ipts2, originalMatches);
	float_t totalTimeElapsed = (float_t) (clock() - tStart) / CLOCKS_PER_SEC;
	LOG(stdout, "TIME", "CPU time of (deprecated) C open surf matcher: %f s", totalTimeElapsed);
	LOG_INFO("Nb Matches by original: %d", (uint32_t) originalMatches.size());

	// Convert back and compare
	GBL::MatchCollection_t openMatches;
	ck_assert_int_eq(convertToMatchCollection(originalMatches, openMatches), GBL::RESULT_SUCCESS);
	matchesAreEqual(foundMatches, openMatches);
}

START_TEST (test_open_surf_matcher_simple)
{
	LOG_ENTER("Starting test_open_surf_matcher_simple")
	Match::OpenSurfMatcher matcher;
	GBL::Descriptor_t testDescriptor1(2,IPOINT_DESCRIPTOR_SIZE,CV_32FC1);
	// Fill descriptor
	for(int32_t i = 0; i < testDescriptor1.rows; i++) {
		for(int32_t j = 0; j < testDescriptor1.cols; j++) {
			testDescriptor1.at<float>(i, j) = (float_t) (i + j);
		}
	}
	
	// Construct correct result
	GBL::MatchCollection_t correctMatches;
	for(int32_t i = 0; i < testDescriptor1.rows; i++) {
		GBL::Match_t match;
		match.queryIdx = i;
		match.trainIdx = i;
		correctMatches.push_back(match);
	}	

	// Perform match
	GBL::MatchCollection_t foundMatches;
	matcher.match(testDescriptor1, testDescriptor1, foundMatches);
	
	// Check
	matchesAreEqual(foundMatches, correctMatches);
	correctMatches.clear();

	// Check using the original detector
	compareWithOriginalOpenSurfMatcher(testDescriptor1, testDescriptor1, foundMatches);
	foundMatches.clear();
	
	// Make a second descriptor
	GBL::Descriptor_t testDescriptor2(3,IPOINT_DESCRIPTOR_SIZE,CV_32FC1);
	for(int32_t i = 0; i < testDescriptor2.cols;i++) {
		testDescriptor2.at<float>(0,i) = testDescriptor1.at<float>(0,i);
	}
	for(int32_t i = 0; i < testDescriptor2.cols; i++) {
		testDescriptor2.at<float>(1,i) = testDescriptor1.at<float>(1,i) + 1.0f;
		testDescriptor2.at<float>(2,i) = testDescriptor1.at<float>(2,i) + 1.5f;
	}

	// Construct correct result
	GBL::Match_t match;
	match.queryIdx = 0;
	match.trainIdx = 0;
	correctMatches.push_back(match);
	
	// Perform match
	matcher.match(testDescriptor1, testDescriptor2, foundMatches);	
	LOG_INFO("%d", (uint32_t) foundMatches.size());

	// Check
	compareWithOriginalOpenSurfMatcher(testDescriptor1, testDescriptor2, foundMatches);
//	matchesAreEqual(foundMatches, correctMatches);
	foundMatches.clear();
	correctMatches.clear();

	// Do a big test with random values
	const int32_t nbDescriptors = 128;
	GBL::Descriptor_t testDescriptor3(nbDescriptors, IPOINT_DESCRIPTOR_SIZE, CV_32FC1);
	GBL::Descriptor_t testDescriptor4(nbDescriptors, IPOINT_DESCRIPTOR_SIZE, CV_32FC1);
	srand(0);
	for(int32_t i = 0; i < nbDescriptors; i++) {
		for(int32_t j = 0; j < IPOINT_DESCRIPTOR_SIZE; j++) {
			// Fill with random float values between 0 and 1000
			testDescriptor3.at<float>(i,j) = (rand()/(float) RAND_MAX) * 1000.0f;
			testDescriptor4.at<float>(i,j) = (rand()/(float) RAND_MAX) * 1000.0f;
		}
	}
	// Lets introduce a percentage of matches in there
	for(int32_t i = 0; i < (int32_t) (0.2f * nbDescriptors); i++) {
		uint32_t index = rand() % IPOINT_DESCRIPTOR_SIZE;
		for(int32_t j = 0; j < IPOINT_DESCRIPTOR_SIZE; j++) {
			testDescriptor4.at<float>(index, j) = testDescriptor3.at<float>(index, j);
		}
	}

	// Perform match
	clock_t tStart = clock();
	matcher.match(testDescriptor3, testDescriptor4, foundMatches);
	float_t totalTimeElapsed = (float_t) (clock() - tStart) / CLOCKS_PER_SEC;
	LOG(stdout, "TIME", "CPU time of C++ open surf matcher: %f s", totalTimeElapsed);

	// Check
	compareWithOriginalOpenSurfMatcher(testDescriptor3, testDescriptor4, foundMatches);
	LOG_INFO("Number of matches: %d", (uint32_t)foundMatches.size());
	foundMatches.clear();
	LOG_EXIT("End of test_open_surf_matcher_simple");
}
END_TEST

Suite * testOpenSurfMatcher_suite (void)
{
	Suite *s = suite_create ("Test openSurf Matcher");
	
	TCase *tc_core = tcase_create ("Test openSurf matcher simple");
	tcase_add_test (tc_core, test_open_surf_matcher_simple);
	tcase_set_timeout(tc_core, 30);
	suite_add_tcase (s, tc_core);
	
	return s;
}
