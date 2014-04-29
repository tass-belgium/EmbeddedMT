/*
 * =====================================================================================
 *
 *       Filename:  utils.cpp
 *
 *    Description:  Utils base class
 *
 *        Version:  1.0
 *        Created:  04/28/2014 10:16:59 AM
 *       Revision:  none
 *
 *         Author:  Bart Verhagen (bv), bart.verhagen@tass.be
 *   Organization:  TASS
 *
 * =====================================================================================
 */

#include "log/logging.hpp"
#include "utils.hpp"
#include "outputMethod/outputImageSequence.hpp"

namespace Utils {

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  drawResults
 *  Description: Draws correspondences between two consecutive frames to output folder 
 * =====================================================================================
 */
GBL::CmRetCode_t Utils::drawResults(InputMethod::InputMethodInterface& inputMethod, Draw::DrawInterface& drawer, const ImageProc::ImageProc& imProc, GBL::DescriptorContainer_t* descriptors, GBL::MatchesContainer_t* allMatches, uint32_t nbFrames, const GBL::Image_t& image2) {
	LOG_ENTER("void");
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	OutputMethod::OutputImageSequence outputMethod;
	const char* outputFile = "correspondenceframe";
	LOG_INFO("Drawing output images");
	if(drawHelper(outputMethod, inputMethod, drawer, imProc, descriptors, allMatches, nbFrames, outputFile, nullptr, nullptr) != GBL::RESULT_SUCCESS) {
		LOG_ERROR("Could not draw result frames");
	}
	const char* procOutputFile = "procframe";
	DrawResultProc_f fastSubtract_f = fastSubtractHandler;
	if(drawHelper(outputMethod, inputMethod, drawer, imProc, descriptors, allMatches, nbFrames, procOutputFile, fastSubtract_f, &image2) != GBL::RESULT_SUCCESS) {
		LOG_ERROR("Could not draw processing frames");
	}
	LOG_EXIT("result = %d", result);
	return result;
}		/* -----  end of function drawResults  ----- */

void Utils::fastSubtractHandler(const ImageProc::ImageProc& imProc, const GBL::Image_t& image1, const GBL::Image_t& image2, GBL::Image_t& outputImage) {
	imProc.fastSubtract(image1, image2, outputImage);
}

GBL::CmRetCode_t Utils::drawHelper(OutputMethod::OutputMethodInterface& outputMethod, InputMethod::InputMethodInterface& inputMethod, Draw::DrawInterface& drawer, const ImageProc::ImageProc& imProc, GBL::DescriptorContainer_t* descriptors, GBL::MatchesContainer_t* allMatches, uint32_t nbFrames, const char* outputFile,DrawResultProc_f procFunction, const GBL::Image_t* const image2) {
	LOG_ENTER("Drawing %s files", outputFile);
	if(outputMethod.open(outputFile) != GBL::RESULT_SUCCESS) {
		LOG_ERROR("Could not open %s", outputFile);
		return GBL::RESULT_FAILURE;
	}
	GBL::Frame_t preFrame1;
	GBL::Frame_t frame1;
	if (inputMethod.getFrame(0, preFrame1) != GBL::RESULT_SUCCESS) {
		LOG_ERROR("Could not get frame %d", 0);
	}
	if(procFunction != nullptr) {
		procFunction(imProc, preFrame1, *image2, frame1);
	} else {
		frame1 = preFrame1;
	}
	for (uint32_t j = 0; j < nbFrames-1; j++) {
		LOG_INFO("Generating frame %d", j+1);
		GBL::Frame_t preFrame2;
		if (inputMethod.getFrame(j+1, preFrame2) != GBL::RESULT_SUCCESS) {
			LOG_ERROR("Could not get frame %d", j+1);
			continue;
		}

		GBL::Image_t frame2;
		if(procFunction != nullptr && image2 != nullptr) {
			procFunction(imProc, preFrame2, *image2, frame2);
		} else {
			frame2 = preFrame2;
		}
		GBL::Image_t outputFrame;
		drawer.draw(frame1, frame2, allMatches[j].matches, descriptors[j].keypoints, descriptors[j + 1].keypoints, outputFrame);
		outputMethod.write(outputFrame);
		frame1 = frame2;
	}
	drawer.closeFile();
	if(outputMethod.close() != GBL::RESULT_SUCCESS) {
		LOG_WARNING("Could not close %s", outputFile);
	}
	LOG_EXIT("Done drawing %s", outputFile);
	return GBL::RESULT_SUCCESS;
}
}
