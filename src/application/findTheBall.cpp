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

#include "cm/global.hpp"
#include "log/logging.hpp"
#include "findTheBall.hpp"

const bool drawResults = false;
const bool denoise = false;
const bool sharpen = false;
const bool subtractBackground = true;

float_t timeElapsedOnBackground = 0;
static GBL::Image_t background;

typedef struct DescriptorContainer {
	bool valid;
	GBL::Descriptor_t descriptor;
	GBL::KeyPointCollection_t keypoints;
} DescriptorContainer_t;

static GBL::CmRetCode_t getImage(GBL::Image_t& img, const char* const imagePath, const ImageProc::ImageProc* imProc);

std::vector<GBL::Displacement_t> findTheBall(GBL::ImageSequence_t& imageSequence, const ImageProc::ImageProc* imProc, const Draw::DrawInterface& drawer,
		const Descriptor::DescriptorInterface& descriptor, const Match::MatcherInterface& matcher, const Displacement::DisplacementInterface& displacement) {
	LOG_ENTER("void");

	// Get background image
	if (subtractBackground) {
		LOG_INFO("Retrieving background image %s", imageSequence.backgroundImage);
		background = cv::imread(imageSequence.backgroundImage, CV_LOAD_IMAGE_GRAYSCALE);
		//		imProc->denoise(background, background);
	}

	DescriptorContainer_t descriptors[imageSequence.images.size()];
#pragma omp parallel shared(descriptors)
	{
#pragma omp for schedule(static) nowait
		for (uint32_t i = 0; i < imageSequence.images.size(); i++) {
			GBL::Image_t img;

			// Get image
			LOG_INFO("Getting image %d = %s", i, imageSequence.images[i].c_str());
			if (getImage(img, imageSequence.images[i].c_str(), imProc) != GBL::RESULT_SUCCESS) {
				LOG_ERROR("Could not get image %s", imageSequence.images[0].c_str());
				descriptors[i].valid = false;
				continue;
			}
			descriptors[i].valid = true;
			// Describe image
			LOG_INFO("Describing image %d", i);
			descriptor.describe(img, descriptors[i].keypoints, descriptors[i].descriptor);
		}
	}

	GBL::MatchCollection_t allMatches[imageSequence.images.size() - 1];
#pragma omp parallel shared(descriptors)
	{
#pragma omp for schedule(static) nowait
		for (uint32_t i = 0; i < imageSequence.images.size() - 1; i++) {
			// Match images
			LOG_INFO("Matching image %d with image %d", i, i + 1);
			matcher.match(descriptors[i].descriptor, descriptors[i + 1].descriptor, allMatches[i]);
			LOG_INFO("Found matches for image %d and image %d: %d", i, i + 1, (uint32_t ) allMatches[i].size());
		}
	}

	if (drawResults) {
		for (uint32_t i = 0; i < imageSequence.images.size() - 1; i++) {
			GBL::Image_t image1;
			GBL::Image_t image2;
			if (getImage(image1, imageSequence.images[i].c_str(), imProc) != GBL::RESULT_SUCCESS) {
				LOG_ERROR("Could not get image %s", imageSequence.images[i].c_str());
				continue;
			}
			if (getImage(image2, imageSequence.images[i + 1].c_str(), imProc) != GBL::RESULT_SUCCESS) {
				LOG_ERROR("Could not get image %s", imageSequence.images[i + 1].c_str());
				continue;
			}
			drawer.draw(image1, image2, allMatches[i], descriptors[i].keypoints, descriptors[i + 1].keypoints);
			cv::waitKey(0);
		}
	}

	// Get displacements
	std::vector<GBL::Displacement_t> displacements(imageSequence.images.size() - 1);
	for (uint32_t i = 0; i < imageSequence.images.size() - 1; i++) {
		LOG_INFO("Calculating displacement between image %d and image %d", i, i + 1);
		if (displacement.calculateDisplacement(allMatches[i], descriptors[i].keypoints, descriptors[i + 1].keypoints, displacements[i])
				!= GBL::RESULT_SUCCESS) {
			LOG_ERROR("Could not find displacement for images %s and %s", imageSequence.images[i].c_str(), imageSequence.images[i + 1].c_str());
			displacements[i].x = 0;
			displacements[i].y = 0;
			continue;
		}
		LOG_INFO("x displacement: %d", displacements[i].x);
		LOG_INFO("y displacement: %d", displacements[i].y);
	}
	LOG_EXIT("Displacements = %p, size = %d", &displacements, (uint32_t ) displacements.size());
	return displacements;
}

GBL::CmRetCode_t getImage(GBL::Image_t& img, const char* const imagePath, const ImageProc::ImageProc* imProc) {
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
