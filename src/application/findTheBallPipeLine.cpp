/*
 * =====================================================================================
 *
 *       Filename:  findTheBallPipeLine.cpp
 *
 *    Description:  Pipe lines version of find the ball
 *
 *        Version:  1.0
 *        Created:  04/28/2014 10:03:32 AM
 *       Revision:  none
 *
 *         Author:  Bart Verhagen (bv), bart.verhagen@tass.be
 *   Organization:  TASS
 *
 * =====================================================================================
 */
#include "cm/global.hpp"
#include "cm/utils.hpp"
#include "log/logging.hpp"
#include "findTheBall.hpp"

#include "outputMethod/outputImageSequence.hpp"
#include "findTheBallPipeLine.hpp"

const bool subtractBackground = true;
static GBL::Frame_t background;

void descriptionHelper(const GBL::Image_t& frameToDescribe, GBL::DescriptorContainer_t& descriptor, const Descriptor::DescriptorInterface& descriptorInterface); 
void matcherHelper(const GBL::DescriptorContainer_t& descriptor1, const GBL::DescriptorContainer_t& descriptor2, GBL::MatchesContainer_t& matches, const Match::MatcherInterface& matcherInterface); 
void displacementHelper(const GBL::DescriptorContainer_t& descriptor1, const GBL::DescriptorContainer_t& descriptor2, const GBL::MatchesContainer_t& matches, GBL::Displacement_t& displacement, const Displacement::DisplacementInterface& displacementInterface, OutputMethod::OutputMethodInterface& outputMethodInterface); 

std::vector<GBL::Displacement_t> findTheBallPipeline(const char* const videoFile, const ImageProc::ImageProc* imProc, Draw::DrawInterface& drawer,
		const Descriptor::DescriptorInterface& descriptorInterface, const Match::MatcherInterface& matcherInterface, const Displacement::DisplacementInterface& displacementInterface,
		InputMethod::InputMethodInterface& inputMethodInterface, OutputMethod::OutputMethodInterface& outputMethodInterface) {
	
	// Single threaded initialization phase
	if (inputMethodInterface.start(videoFile) != GBL::RESULT_SUCCESS) {
		LOG_ERROR("Could not open %s", videoFile);
		return std::vector < GBL::Displacement_t > (0);
	}

	// Get background image
	if (subtractBackground) {
		LOG_INFO("Retrieving background");
		// For now take first frame as the background
		if (inputMethodInterface.getNextFrame(background) != GBL::RESULT_SUCCESS) {
			LOG_ERROR("Could not get background");
			return std::vector < GBL::Displacement_t > (0);
		}
	}
	const uint32_t nbFrames = inputMethodInterface.size() - 1; // Subtract the background from the sequence

	GBL::DescriptorContainer_t descriptors[nbFrames];
	GBL::MatchesContainer_t matches[nbFrames-1];
	std::vector<GBL::Displacement_t> displacements(nbFrames - 1);

	LOG_INFO("Processing frames");
	GBL::Frame_t* frame = new GBL::Frame_t;
	uint32_t i = 0;
	while(inputMethodInterface.getNextFrame(*frame) == GBL::RESULT_SUCCESS) {
		// There is a next frame
		GBL::Frame_t* newFrame = new GBL::Frame_t;
		if(subtractBackground) {
			LOG_INFO("Frame size = %d x %d, dim = %d", frame->rows, frame->cols, frame->dims);
			imProc->fastSubtract(*frame, background, *newFrame); 
			LOG_INFO("Frame size = %d x %d, dim = %d", newFrame->rows, newFrame->cols, newFrame->dims);
		}
		
		// Description
		LOG_ENTER("Describing image %d", i); 
		descriptionHelper(*newFrame, descriptors[i], descriptorInterface);	
		
		// Match
		// TODO change this mechanism to a threaded one
		if( i > 0) {
			matcherHelper(descriptors[i-1], descriptors[i], matches[i-1], matcherInterface);
			displacementHelper(descriptors[i-1], descriptors[i], matches[i-1], displacements[i-1], displacementInterface, outputMethodInterface);
		}
		delete newFrame;
		i++;	
	}
	if (GBL::drawResults_b) {
		Utils::Utils::drawResults(inputMethodInterface, drawer, *imProc, descriptors, matches, nbFrames, background);	
	}
	// TODO: Wait for all threads to finish
	
	inputMethodInterface.stop();
	delete frame;
	return displacements;
}

void descriptionHelper(const GBL::Image_t& frameToDescribe, GBL::DescriptorContainer_t& descriptor, const Descriptor::DescriptorInterface& descriptorInterface) {
	LOG_ENTER("Describing frame %p, type = %d, rows = %d, cols = %d, dims = %d", &frameToDescribe, frameToDescribe.type(), frameToDescribe.rows, frameToDescribe.cols, frameToDescribe.dims);
	descriptorInterface.describe(frameToDescribe, descriptor.keypoints, descriptor.descriptor);
	LOG_INFO("Number of found key points = %d, descriptor length = %d", (uint32_t ) descriptor.descriptor.rows, descriptor.descriptor.cols);
	if (descriptor.descriptor.rows == 0) {
		LOG_WARNING("Did not find any keypoints");
		descriptor.valid = false;
	} else {
		descriptor.valid = true;
	}
	LOG_EXIT("void");
}

void matcherHelper(const GBL::DescriptorContainer_t& descriptor1, const GBL::DescriptorContainer_t& descriptor2, GBL::MatchesContainer_t& matches, const Match::MatcherInterface& matcherInterface) {
	LOG_ENTER("descriptor 1 = %p, descriptor 2 = %p, matches = %p, matchesInterface = %p", &descriptor1, &descriptor2, &matches, &matcherInterface);
	if(descriptor1.valid == true && descriptor2.valid == true) {
		matcherInterface.match(descriptor1.descriptor, descriptor2.descriptor, matches.matches);
		matches.valid = (matches.matches.size() > 0);
		LOG_INFO("Nb of found matches: %d", (uint32_t) matches.matches.size());
	} else {
		LOG_WARNING("One or both of the descriptors is invalid");
		matches.valid = false;
	}
	LOG_EXIT("void");
}

void displacementHelper(const GBL::DescriptorContainer_t& descriptor1, const GBL::DescriptorContainer_t& descriptor2, const GBL::MatchesContainer_t& matches, GBL::Displacement_t& displacement, const Displacement::DisplacementInterface& displacementInterface, OutputMethod::OutputMethodInterface& outputMethodInterface) {
	LOG_ENTER("Descriptor 1 = %p, descriptor2 = %p, matches = %p, displacement = %p, displacementInterface = %p", &descriptor1, &descriptor2, &matches, &displacement, &displacementInterface);
	if(matches.valid) {
		if(displacementInterface.calculateDisplacement(matches.matches, descriptor1.keypoints, descriptor2.keypoints, displacement) != GBL::RESULT_SUCCESS) {
			LOG_ERROR("Could not find displacement");
			displacement.x = 0;
			displacement.y = 0;
		}
	} else {
		LOG_WARNING("Invalid matches");
	}
	LOG_INFO("Writing displacement to socket");
	outputMethodInterface.write(displacement);
	LOG_EXIT("void");
}	

