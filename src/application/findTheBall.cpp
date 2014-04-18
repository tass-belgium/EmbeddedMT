/*
 * findTheBallVideo.cpp
 *
 *  Created on: Apr 17, 2014
 *      Author: cv
 */

/*
 * findTheBall.cpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */
#include <string>
#include <cv.h>
#include <highgui.h>

#include "cm/global.hpp"
#include "log/logging.hpp"
#include "findTheBall.hpp"

const bool drawResults = true;
const bool denoise = false;
const bool sharpen = false;
const bool subtractBackground = true;

static GBL::Frame_t background;

typedef struct DescriptorContainer {
	bool valid;
	GBL::Descriptor_t descriptor;
	GBL::KeyPointCollection_t keypoints;
} DescriptorContainer_t;

std::vector<GBL::Displacement_t> findTheBall(const char* const videoFile, const ImageProc::ImageProc* imProc, Draw::DrawInterface& drawer,
		const Descriptor::DescriptorInterface& descriptor, const Match::MatcherInterface& matcher, const Displacement::DisplacementInterface& displacement,
		InputMethod::InputMethodInterface& inputMethod, OutputMethod::OutputMethodInterface& outputMethod) {
	LOG_ENTER("videoFile = %s", videoFile);

	if (inputMethod.start(videoFile) != GBL::RESULT_SUCCESS) {
		LOG_ERROR("Could not open %s", videoFile);
		return std::vector < GBL::Displacement_t > (0);
	}

	// Get background image
	if (subtractBackground) {
		LOG_INFO("Retrieving background");
		// For now take first frame as the background
		if (inputMethod.getNextFrame(background) != GBL::RESULT_SUCCESS) {
			LOG_ERROR("Could not get background");
			return std::vector < GBL::Displacement_t > (0);
		}
	}

	const uint32_t nbFrames = inputMethod.size() - 1; // We need to subtract the background from it
	DescriptorContainer_t descriptors[nbFrames];
//#pragma omp parallel for shared(descriptors) schedule(static)
	for (uint32_t i = 0; i < nbFrames; i++) {
		GBL::Frame_t frame;

		GBL::CmRetCode_t ret;

#pragma omp critical
		{
			ret = inputMethod.getFrame(i, frame);
		}

		if (ret != GBL::RESULT_SUCCESS) {
			LOG_ERROR("Could not get frame %d", i);
			descriptors[i].valid = false;
			continue;
		}
		imProc->fastSubtract(frame, background, frame);
		descriptors[i].valid = true;
		// Describe frame
		LOG_INFO("Describing image %d, type = %d, rows = %d, cols = %d, dims = %d", i, frame.type(), frame.rows, frame.cols, frame.dims);
		descriptor.describe(frame, descriptors[i].keypoints, descriptors[i].descriptor);
		LOG_INFO("Number of found key points = %d, descriptor = %dx%d", (uint32_t ) descriptors[i].keypoints.size(), descriptors[i].descriptor.rows,
				descriptors[i].descriptor.cols);
		if (descriptors[i].keypoints.size() == 0) {
			LOG_WARNING("Did not find any keypoints in frame %d", i);
			descriptors[i].valid = false;
		}
		LOG_INFO("Descriptor type = %d", CV_MAT_TYPE(descriptors[i].descriptor.type()));
	}

	GBL::MatchCollection_t allMatches[nbFrames - 1];
#pragma omp parallel for shared(descriptors, allMatches) schedule(static)
	for (uint32_t i = 0; i < nbFrames - 1; i++) {
		// Match images
		LOG_INFO("Matching image %d with image %d", i, i + 1);
		if (descriptors[i].valid == true && descriptors[i + 1].valid == true) {
			matcher.match(descriptors[i].descriptor, descriptors[i + 1].descriptor, allMatches[i]);
			LOG_INFO("Found matches for image %d and image %d: %d", i, i + 1, (uint32_t ) allMatches[i].size());
		}
	}

	if (drawResults) {
		GBL::Frame_t frame1;
		if (inputMethod.getFrame(0, frame1) != GBL::RESULT_SUCCESS) {
			LOG_ERROR("Could not get frame %d", 0);
		}
		for (uint32_t j = 0; j < nbFrames-1; j++) {
			LOG_INFO("Generating frame %d", j+1);
			GBL::Frame_t frame2;
			if (inputMethod.getFrame(j+1, frame2) != GBL::RESULT_SUCCESS) {
				LOG_ERROR("Could not get frame %d", j+1);
				continue;
			}
			GBL::Frame_t outputFrame;
			drawer.draw(frame1, frame2, allMatches[j], descriptors[j].keypoints, descriptors[j + 1].keypoints, outputFrame);
			outputMethod.write(outputFrame);
			frame1 = frame2;
		}
		drawer.closeFile();
	}
	if (inputMethod.stop() != GBL::RESULT_SUCCESS) {
		LOG_ERROR("Could not close %s", videoFile);
	}

	// Get displacements
	std::vector<GBL::Displacement_t> displacements(nbFrames - 1);
	for (uint32_t i = 0; i < nbFrames - 1; i++) {
		LOG_INFO("Calculating displacement between image %d and image %d", i, i + 1);
		if (descriptors[i].valid == true && descriptors[i + 1].valid == true) {
			if (displacement.calculateDisplacement(allMatches[i], descriptors[i].keypoints, descriptors[i + 1].keypoints, displacements[i])
					!= GBL::RESULT_SUCCESS) {
				LOG_ERROR("Could not find displacement for frame %d and %d", i, i + 1);
				displacements[i].x = 0;
				displacements[i].y = 0;
				continue;
			}
		} else {
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
