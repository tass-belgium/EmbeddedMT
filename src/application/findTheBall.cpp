/*
 * findTheBall.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */
#include <iostream>
#include <string>
#include <stdio.h>
#include <cv.h>
#include <highgui.h>

#include "log/logging.hpp"
#include "findTheBall.hpp"

const bool drawResults = false;
const bool denoise = false;
const bool sharpen = false;
const bool subtractBackground = true;

float_t timeElapsedOnBackground = 0;
static GBL::Image_t background;

GBL::CmRetCode_t getImage(GBL::Image_t& img, const char* const imagePath,
		const ImageProc::ImageProc* imProc);
GBL::CmRetCode_t match(const Descriptor::DescriptorInterface& descriptor,
		const Match::MatcherInterface& matcher, GBL::Image_t& img1,
		GBL::Image_t& img2, const GBL::KeyPointCollection_t& keypoints1, const GBL::Descriptor_t& descriptor1,
		GBL::KeyPointCollection_t& keypoints2, GBL::Descriptor_t& descriptor2,
		GBL::MatchCollection_t& matches);

std::vector<GBL::Displacement_t> findTheBall(
		GBL::ImageSequence_t& imageSequence, const ImageProc::ImageProc* imProc,
		const Draw::DrawInterface& drawer,
		const Descriptor::DescriptorInterface& descriptor,
		const Match::MatcherInterface& matcher,
		const Displacement::DisplacementInterface& displacement) {
	LOG_ENTER("void");
	std::vector<GBL::Displacement_t> displacements(imageSequence.images.size());
	GBL::Image_t img1;
	GBL::Image_t img2;

	if (imageSequence.images.size() == 0) {
		return displacements;
	}

	// Get background image
	if (subtractBackground) {
		LOG_INFO("Retrieving background image %s", imageSequence.backgroundImage);
		background = cv::imread(imageSequence.backgroundImage,
				CV_LOAD_IMAGE_GRAYSCALE);
		//		imProc->denoise(background, background);
	}

	// Get very first image
	LOG_INFO("Writing image %s to %p", imageSequence.images[0].c_str(), &img1);
	if (getImage(img1, imageSequence.images[0].c_str(), imProc)
			!= GBL::RESULT_SUCCESS) {
		LOG_ERROR("Could not get image %s", imageSequence.images[0].c_str());
		return displacements;
	}
	// Describe first image
	GBL::KeyPointCollection_t keypoints1;
	GBL::Descriptor_t descriptor1;
	LOG_INFO("Describing img1 = %p", &img1);
	descriptor.describe(img1, keypoints1, descriptor1);

	for (uint32_t i = 0; i < imageSequence.images.size() - 1; i++) {
		uint32_t img2Index = i + 1;
		if (getImage(img2, imageSequence.images[img2Index].c_str(), imProc)
				!= GBL::RESULT_SUCCESS) {
			LOG_ERROR("Could not get image %s",
					imageSequence.images[img2Index].c_str());
			break;
		}

		GBL::KeyPointCollection_t keypoints2;
		GBL::Descriptor_t descriptor2;
		GBL::MatchCollection_t matches;

		LOG_INFO("Matching %s and %s", imageSequence.images[i].c_str(), imageSequence.images[i+1].c_str());
		if (match(descriptor, matcher, img1, img2, keypoints1, descriptor1, keypoints2, descriptor2,
				matches) != GBL::RESULT_SUCCESS) {
			LOG_ERROR("Could not match images %s and %s",
					imageSequence.images[i].c_str(),
					imageSequence.images[img2Index].c_str());
			displacements[i].x = 0;
			displacements[i].y = 0;
			continue;
		}
		for (uint32_t j = 0; j < matches.size(); j++) {
			LOG_DEBUG("[%f,%f] with [%f,%f]",
					keypoints1[matches[j].queryIdx].pt.x,
					keypoints1[matches[j].queryIdx].pt.y,
					keypoints2[matches[j].trainIdx].pt.x,
					keypoints2[matches[j].trainIdx].pt.y);
		}
		if (drawResults) {
			drawer.draw(img1, img2, matches, keypoints1, keypoints2);
			cv::waitKey(0);
		}

		/* Calculate displacement */
		if (displacement.calculateDisplacement(matches, keypoints1, keypoints2,
				displacements[i]) != GBL::RESULT_SUCCESS) {
			LOG_ERROR("Could not find displacement for images %s and %s",
					imageSequence.images[i].c_str(),
					imageSequence.images[img2Index].c_str());
			displacements[i].x = 0;
			displacements[i].y = 0;
			continue;
		}
		LOG_INFO("x displacement: %d", displacements[i].x);
		LOG_INFO("y displacement: %d", displacements[i].y);

		// Assign image 2 to image 1
		img1 = img2;
		keypoints1 = keypoints2;
		descriptor1 = descriptor2;
	}
	LOG_EXIT("Displacements = %p, size = %d", &displacements, (uint32_t) displacements.size());
	return displacements;
}

GBL::CmRetCode_t getImage(GBL::Image_t& img, const char* const imagePath,
		const ImageProc::ImageProc* imProc) {
	LOG_ENTER("imagePath = %s, output image = %p", imagePath, &img);
	img = cv::imread(imagePath, CV_LOAD_IMAGE_GRAYSCALE);

	if (!img.data) {
		std::cout << " --(!) Error reading images " << std::endl;
		return GBL::RESULT_FAILURE;
	}

	if (subtractBackground) {
		imProc->fastSubtract(img, background, img);
	}

// Denoise image
	if (denoise) {
		imProc->denoise(img, img);
	}

// Sharpen image
	if (sharpen) {
		const double_t sigmaX = 3;
		const double_t beta = -0.5;
		imProc->sharpen(img, img, sigmaX, beta);
	}
	GBL::CmRetCode_t result = GBL::RESULT_SUCCESS;
	LOG_EXIT("Result = %d", result);
	return result;
}

GBL::CmRetCode_t match(const Descriptor::DescriptorInterface& descriptor,
		const Match::MatcherInterface& matcher, GBL::Image_t& img1,
		GBL::Image_t& img2, const GBL::KeyPointCollection_t& keypoints1, const GBL::Descriptor_t& descriptor1,
		GBL::KeyPointCollection_t& keypoints2, GBL::Descriptor_t& descriptor2,
		GBL::MatchCollection_t& matches) {
	LOG_ENTER("img2 = %p, keypoints 1 = %p, keypoints 2 = %p, matches = %p", &img2, &keypoints1, &keypoints2, &matches);

	LOG_INFO("Nb of keypoints in image 1: %d", (uint32_t) keypoints1.size());
	if (keypoints1.size() == 0) {
		LOG_EXIT("GBL::RESULT_FAILURE");
		return GBL::RESULT_FAILURE;
	}

	LOG_INFO("Describing image 2");
	descriptor.describe(img2, keypoints2, descriptor2);
	LOG_INFO("Nb of keypoints in image 2: %d", (uint32_t) keypoints2.size());
	if (keypoints2.size() == 0) {
		LOG_EXIT("GBL::RESULT_FAILURE");
		return GBL::RESULT_FAILURE;
	}

	LOG_INFO("Matching");
	matcher.match(descriptor1, descriptor2, matches);
	LOG_INFO("Nb of found matches: %d", (uint32_t) matches.size());
	if (matches.size() == 0) {
		LOG_EXIT("GBL::RESULT_FAILURE");
		return GBL::RESULT_FAILURE;
	}
	LOG_EXIT("GBL::RESULT_SUCCESS");
	return GBL::RESULT_SUCCESS;
}
