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

#include "findTheBall.hpp"

const bool drawResults = false;
const bool denoise = false;
const bool sharpen = false;
const bool verbose = false;
const bool subtractBackground = true;

float_t timeElapsedOnBackground = 0;
static GBL::Image_t background;

GBL::CmRetCode_t getImage(GBL::Image_t& img, const char* const imagePath,
		const ImageProc::ImageProc* imProc);
GBL::CmRetCode_t match(const Descriptor::DescriptorInterface& descriptor,
		const Match::MatcherInterface& matcher, GBL::Image_t& img1,
		GBL::Image_t& img2, GBL::KeyPointCollection_t& keypoints1,
		GBL::KeyPointCollection_t& keypoints2, GBL::MatchCollection_t& matches);

std::vector<GBL::Displacement_t> findTheBall(GBL::ImageSequence_t& imageSequence, const ImageProc::ImageProc* imProc,
		const Draw::DrawInterface& drawer,
		const Descriptor::DescriptorInterface& descriptor,
		const Match::MatcherInterface& matcher,
		const Displacement::DisplacementInterface& displacement) {
	std::vector<GBL::Displacement_t> displacements(imageSequence.images.size());
	GBL::Image_t img1;
	GBL::Image_t img2;

	if(imageSequence.images.size() == 0) {
		return displacements;
	}

	/* Get background image */
	if (subtractBackground) {
		background = cv::imread(imageSequence.backgroundImage,
				CV_LOAD_IMAGE_GRAYSCALE);
		//		imProc->denoise(background, background);
	}

	/* Get very first image */
	if (getImage(img1, imageSequence.images[0].c_str(), imProc) != GBL::RESULT_SUCCESS) {
		std::cerr << "Could not get image " << imageSequence.images[0] << std::endl;
		return displacements;
	}

	for (uint32_t i = 0; i < imageSequence.images.size()-1; i++) {
		uint32_t img2Index = i+1;
		if (getImage(img2, imageSequence.images[img2Index].c_str(), imProc) != GBL::RESULT_SUCCESS) {
			std::cerr << "Could not get image " << imageSequence.images[img2Index] << std::endl;
			return std::vector<GBL::Displacement_t>(0);
		}

		GBL::KeyPointCollection_t keypoints1;
		GBL::KeyPointCollection_t keypoints2;
		GBL::MatchCollection_t matches;

		if (match(descriptor, matcher, img1, img2, keypoints1, keypoints2,
				matches) != GBL::RESULT_SUCCESS) {
			std::cerr << "Could not match images " << imageSequence.images[i] << " and " << imageSequence.images[i+1] << std::endl;
			displacements[i].x = 0;
			displacements[i].y = 0;
			continue;
		}
		if (verbose) {
			for (uint32_t i = 0; i < matches.size(); i++) {
				std::cout << "[" << keypoints1[matches[i].queryIdx].pt.x << ','
						<< keypoints1[matches[i].queryIdx].pt.y << "] with ["
						<< keypoints2[matches[i].trainIdx].pt.x << ","
						<< keypoints2[matches[i].trainIdx].pt.y << "]"
						<< std::endl;
			}
		}
		if (drawResults) {
			drawer.draw(img1, img2, matches, keypoints1, keypoints2);
			cv::waitKey(0);
		}

		/* Calculate displacement */
		float_t x = 0;
		float_t y = 0;
		if(displacement.calculateDisplacement(matches, keypoints1, keypoints2, displacements[i]) != GBL::RESULT_SUCCESS) {
			std::cerr << "Could not find displacement for images " << imageSequence.images[i] << " and " << imageSequence.images[i+1] << std::endl;
			displacements[i].x = 0;
			displacements[i].y = 0;
			continue;
		}
		if (verbose) {
			std::cout << "x displacement: " << x << std::endl;
			std::cout << "y displacement: " << y << std::endl;
			std::cout << i << std::endl;
		}
		img1 = img2;
	}
	return displacements;
}

GBL::CmRetCode_t getImage(GBL::Image_t& img, const char* const imagePath,
		const ImageProc::ImageProc* imProc) {
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
	return GBL::RESULT_SUCCESS;
}

GBL::CmRetCode_t match(const Descriptor::DescriptorInterface& descriptor,
		const Match::MatcherInterface& matcher, GBL::Image_t& img1,
		GBL::Image_t& img2, GBL::KeyPointCollection_t& keypoints1,
		GBL::KeyPointCollection_t& keypoints2, GBL::MatchCollection_t& matches) {

	GBL::Descriptor_t descriptor1;
	GBL::Descriptor_t descriptor2;

	uint8_t scaleLevels = 1;

	// Describe
	descriptor.describe(img1, scaleLevels, keypoints1, descriptor1);
	if (verbose) {
		std::cout << "Nb of keypoints in image 1: " << keypoints1.size()
				<< std::endl;
	}
	if (keypoints1.size() == 0) {
		return GBL::RESULT_FAILURE;
	}

	descriptor.describe(img2, scaleLevels, keypoints2, descriptor2);
	if (verbose) {
		std::cout << "Nb of keypoints in image 2: " << keypoints2.size()
				<< std::endl;
	}
	if (keypoints2.size() == 0) {
		return GBL::RESULT_FAILURE;
	}

	// Match
	matcher.match(descriptor1, descriptor2, matches);
	if (verbose) {
		std::cout << "Nb of found matches: " << matches.size() << std::endl;
	}
	if (matches.size() == 0) {
		return GBL::RESULT_FAILURE;
	}
	return GBL::RESULT_SUCCESS;
}
