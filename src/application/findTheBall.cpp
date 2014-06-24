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

#include "cm/global.hpp"
#include "cm/utils.hpp"
#include "log/logging.hpp"
#include "findTheBall.hpp"

#include "outputMethod/outputImageSequence.hpp"

const bool denoise = false;
const bool sharpen = false;
const bool subtractBackground = true;

static GBL::Frame_t background;

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

	const uint32_t nbFrames = inputMethod.size() - 1; // Subtract the background from the sequence
	std::vector<GBL::DescriptorContainer_t> descriptors(nbFrames);
	for (uint32_t i = 0; i < nbFrames; i++) {
		GBL::Frame_t inputFrame;

		GBL::CmRetCode_t ret;
		ret = inputMethod.getFrame(i, inputFrame);

		if (ret != GBL::RESULT_SUCCESS) {
			LOG_ERROR("Could not get frame %d", i);
			descriptors[i].valid = false;
			continue;
		}
		GBL::Image_t frame;
		if(subtractBackground) {
			imProc->fastSubtract(inputFrame, background, frame);
		} else {
			frame = inputFrame;
		}
		if(denoise) {
			imProc->denoise(frame, frame);
		}
		if(sharpen) {
			imProc->sharpen(frame, frame, 0.5, -3);
		}
		descriptors[i].valid = true;
		// Describe frame
		LOG_INFO("Describing image %d, type = %d, rows = %d, cols = %d, dims = %d", i, frame.type(), frame.rows, frame.cols, frame.dims);
		descriptor.describe(frame, descriptors[i].keypoints, descriptors[i].descriptor);
		LOG_INFO("Number of found key points = %d, descriptor length = %d", (uint32_t ) descriptors[i].descriptor.rows, descriptors[i].descriptor.cols);
		if (descriptors[i].descriptor.rows == 0) {
			LOG_WARNING("Did not find any keypoints in frame %d", i);
			descriptors[i].valid = false;
		}
		LOG_INFO("Descriptor type = %d", CV_MAT_TYPE(descriptors[i].descriptor.type()));
	}

	std::vector<GBL::MatchesContainer_t> allMatches(nbFrames - 1);
	for (uint32_t i = 0; i < nbFrames - 1; i++) {
		uint32_t nextImageIndex = i + 1;
		allMatches[i].valid = false;
		// Match images
		if (descriptors[i].valid == true && descriptors[nextImageIndex].valid == true) {
			LOG_INFO("Matching image %d with image %d", i, nextImageIndex);
			matcher.match(descriptors[i].descriptor, descriptors[nextImageIndex].descriptor, allMatches[i].matches);
			LOG_INFO("Nb of found matches for image %d and image %d: %d", i, nextImageIndex, (uint32_t ) allMatches[i].matches.size());
			if(allMatches[i].matches.size() == 0) {
				LOG_WARNING("Did not find any matches between frame %d and frame %d", i, nextImageIndex);
			} else {
				allMatches[i].valid = true;
			}
		} else {
			LOG_INFO("Not matching image %d and %d since one or both are flagged invalid.", i, nextImageIndex);
			nextImageIndex++;
		}
	}

	if (GBL::drawResults_b) {
		Utils::Utils::drawResults(inputMethod, drawer, *imProc, descriptors, allMatches, nbFrames, background);	
	}
	if (inputMethod.stop() != GBL::RESULT_SUCCESS) {
		LOG_ERROR("Could not close %s", videoFile);
	}

	// Get displacements
	std::vector<GBL::Displacement_t> displacements(nbFrames - 1);
	for (uint32_t i = 0; i < nbFrames - 1; i++) {
		if (allMatches[i].valid == true) {
			LOG_INFO("Calculating displacement between image %d and image %d", i, i + 1);
			if (displacement.calculateDisplacement(allMatches[i].matches, descriptors[i].keypoints, descriptors[i + 1].keypoints, displacements[i])
					!= GBL::RESULT_SUCCESS) {
				LOG_ERROR("Could not find displacement for frame %d and %d", i, i + 1);
				displacements[i].x = 0;
				displacements[i].y = 0;
				continue;
			}
		} else {
			LOG_INFO("Not calculating displacement between frame %d and %d since one or both are not valid", i, i+1);
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


