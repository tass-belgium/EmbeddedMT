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
#include <unistd.h>
#include "cm/global.hpp"
#include "cm/utils.hpp"
#include "log/logging.hpp"
#include "findTheBall.hpp"

#include "outputMethod/outputImageSequence.hpp"
#include "findTheBallPipeLine.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

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
	const uint32_t nbFrames = 50;

	GBL::DescriptorContainer_t descriptors[nbFrames];
	GBL::MatchesContainer_t matches[nbFrames];
	std::vector<GBL::Displacement_t> displacements(nbFrames);

	LOG_INFO("Processing frames");
	GBL::Frame_t* frame = new GBL::Frame_t;
	uint32_t i = 0;
	uint32_t sequenceNo = 0;
#pragma omp parallel shared(inputMethodInterface, descriptorInterface, matcherInterface, displacementInterface, outputMethodInterface, background, displacements, descriptors, matches, imProc, frame, i, sequenceNo)
{	

	#pragma omp single nowait
	while(inputMethodInterface.isMoreInput()) {
		if(inputMethodInterface.getNextFrame(*frame) != GBL::RESULT_SUCCESS) {
			// Lets assume max 30 fps and put the thread to sleep for a while
			usleep(20000);
			continue;
		}
		#pragma omp task firstprivate(i, frame, sequenceNo) shared(descriptors, descriptorInterface, matches, matcherInterface, displacements, displacementInterface, outputMethodInterface, imProc, background) 
		{
			// There is a next frame
			GBL::Frame_t newFrame;
			if(subtractBackground) {
				LOG_INFO("Frame size = %d x %d, dim = %d", frame->rows, frame->cols, frame->dims);
				imProc->fastSubtract(*frame, background, newFrame); 
				LOG_INFO("Frame size = %d x %d, dim = %d", newFrame.rows, newFrame.cols, newFrame.dims);
			} else {
				newFrame = *frame;
			}
			// Description
			LOG_ENTER("Describing image %d", i); 
			descriptors[i].sequenceNo = sequenceNo;
			descriptionHelper(newFrame, descriptors[i], descriptorInterface);

			// Check whether neighbours still exist
			uint32_t prevNeighbourIndex = (i+nbFrames-1) % nbFrames;
			if(descriptors[prevNeighbourIndex].sequenceNo == sequenceNo-1) {
				// Check neighbours whether they are ready
				if(descriptors[prevNeighbourIndex].ready == true) {
					LOG_INFO("Matching %d and %d", prevNeighbourIndex, i);
					matcherHelper(descriptors[prevNeighbourIndex], descriptors[i], matches[prevNeighbourIndex], matcherInterface);
					LOG_INFO("Calculating displacement of %d and %d", prevNeighbourIndex, i);
					displacements[prevNeighbourIndex].sequenceNo = sequenceNo - 1;
					displacementHelper(descriptors[prevNeighbourIndex], descriptors[i], matches[prevNeighbourIndex], displacements[prevNeighbourIndex], displacementInterface, outputMethodInterface);

					if (GBL::drawResults_b) {
						GBL::Frame_t prevNeighbourFrame;
						LOG_INFO("Generating corresponding frame %d and %d", prevNeighbourIndex, i);
						// For the index of getFrame we need to add the background frame again
						if(inputMethodInterface.getFrame(sequenceNo, prevNeighbourFrame) == GBL::RESULT_SUCCESS) {
							Utils::Utils::drawResult(prevNeighbourFrame, *frame, drawer, descriptors[prevNeighbourIndex], descriptors[i], matches[prevNeighbourIndex]);	
						} else {
							LOG_WARNING("Could not get frame of neighbour");
						}
					}
				}
			} else {
				LOG_WARNING("Previous neighbour was someone else");
			}
			uint32_t nextNeighbourIndex = (i+1) % nbFrames;
			if(descriptors[nextNeighbourIndex].sequenceNo == sequenceNo+1) {
				if(descriptors[nextNeighbourIndex].ready == true) {
					LOG_INFO("Matching %d and %d", i, nextNeighbourIndex);
					matcherHelper(descriptors[i], descriptors[nextNeighbourIndex], matches[i], matcherInterface);
					LOG_INFO("Calculating displacement of %d and %d", i, nextNeighbourIndex);
					displacements[i].sequenceNo = sequenceNo;
					displacementHelper(descriptors[i], descriptors[nextNeighbourIndex], matches[i], displacements[i], displacementInterface, outputMethodInterface);
				}
			} else {
				LOG_WARNING("Next neighbour was someone else");
			}
			delete frame;
		}
		sequenceNo++;
		i = (i+1) % nbFrames;
		// Reset the i-th buffers
		descriptors[i].valid = false;
		descriptors[i].ready = false;
		matches[i].valid = false;
		frame = new GBL::Frame_t;
	}
}

	#pragma omp master nowait
	if (GBL::drawResults_b) {
		Utils::Utils::drawResults(inputMethodInterface, drawer, *imProc, descriptors, matches, nbFrames, background);	
	}

	inputMethodInterface.stop();
	delete frame;
	return displacements;
}

void descriptionHelper(const GBL::Image_t& frameToDescribe, GBL::DescriptorContainer_t& descriptor, const Descriptor::DescriptorInterface& descriptorInterface) {
	LOG_ENTER("Describing frame %p, type = %d, rows = %d, cols = %d, dims = %d", &frameToDescribe, frameToDescribe.type(), frameToDescribe.rows, frameToDescribe.cols, frameToDescribe.dims);
	descriptorInterface.describe(frameToDescribe, descriptor.keypoints, descriptor.descriptor);
	LOG_INFO("Number of found key points = %d, descriptor length = %d", (uint32_t ) descriptor.descriptor.rows, descriptor.descriptor.cols);
	LOG_INFO("Describing frame %p, type = %d, rows = %d, cols = %d, dims = %d", &frameToDescribe, frameToDescribe.type(), frameToDescribe.rows, frameToDescribe.cols, frameToDescribe.dims);
	if (descriptor.descriptor.rows == 0) {
		LOG_WARNING("Did not find any keypoints");
		descriptor.valid = false;
	} else {
		LOG_INFO("We did find a couple of keypoints");
		descriptor.valid = true;
	}
	descriptor.ready = true;
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

