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
#include <iostream>

#include "log/logging.hpp"
#include "utils.hpp"
#include "outputMethod/outputImageSequence.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"	
#include "opencv2/highgui/highgui.hpp"

namespace EmbeddedMT {
	namespace Utils {
		GBL::CmRetCode_t Utils::drawResults(InputMethod::InputMethodInterface& inputMethod, Draw::DrawInterface& drawer, const ImageProc::ImageProc& imProc, std::vector<GBL::DescriptorContainer_t> descriptors, std::vector<GBL::MatchesContainer_t> allMatches, uint32_t nbFrames, const GBL::Image_t& image2) {
			LOG_ENTER("void");
			GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
			OutputMethod::OutputImageSequence outputMethod("correspondenceframe");
			LOG_INFO("Drawing output images");
			if(drawHelper(outputMethod, inputMethod, drawer, imProc, descriptors, allMatches, nbFrames, nullptr, nullptr) != GBL::RESULT_SUCCESS) {
				LOG_ERROR("Could not draw result frames");
			}
			OutputMethod::OutputImageSequence procOutputMethod("procframe");
			DrawResultProc_f fastSubtract_f = fastSubtractHandler;
			if(drawHelper(procOutputMethod, inputMethod, drawer, imProc, descriptors, allMatches, nbFrames, fastSubtract_f, &image2) != GBL::RESULT_SUCCESS) {
				LOG_ERROR("Could not draw processing frames");
			}
			LOG_EXIT("result = %d", result);
			return result;
		}

		void Utils::fastSubtractHandler(const ImageProc::ImageProc& imProc, const GBL::Image_t& image1, const GBL::Image_t& image2, GBL::Image_t& outputImage) {
			imProc.fastSubtract(image1, image2, outputImage);
		}

		GBL::CmRetCode_t Utils::drawHelper(OutputMethod::OutputMethodFrameInterface& outputMethod, InputMethod::InputMethodInterface& inputMethod, Draw::DrawInterface& drawer, const ImageProc::ImageProc& imProc, std::vector<GBL::DescriptorContainer_t> descriptors, std::vector<GBL::MatchesContainer_t> allMatches, uint32_t nbFrames, DrawResultProc_f procFunction, const GBL::Image_t* const image2) {
			LOG_ENTER("Drawing files");
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
				// Index of getFrame should compensate for first background image
				if (inputMethod.getFrame(j+2, preFrame2) != GBL::RESULT_SUCCESS) {
					LOG_ERROR("Could not get frame %d", j+1);
					continue;
				}

				GBL::Image_t frame2;
				if(procFunction != nullptr && image2 != nullptr) {
					procFunction(imProc, preFrame2, *image2, frame2);
				} else {
					frame2 = preFrame2;
				}
				if(allMatches[j].valid == true) {
					GBL::Image_t outputFrame;
					drawer.draw(frame1, frame2, allMatches[j].matches, descriptors[j].keypoints, descriptors[j + 1].keypoints, outputFrame);
					outputMethod.write(outputFrame);
				}
				frame1 = frame2;
			}
			drawer.closeFile();
			return GBL::RESULT_SUCCESS;
		}

		GBL::CmRetCode_t Utils::drawResult(const GBL::Frame_t image1, const GBL::Frame_t image2, const Draw::DrawInterface& drawer, GBL::DescriptorContainer_t descriptor1, GBL::DescriptorContainer_t descriptor2, GBL::MatchesContainer_t matches, OutputMethod::OutputMethodFrameInterface* outputMethod) {
			LOG_ENTER("Drawing results");
			GBL::Image_t outputFrame;
			if(matches.valid == true) {
				drawer.draw(image1, image2, matches.matches, descriptor1.keypoints, descriptor2.keypoints, outputFrame);
			} else {
				drawer.draw(image1, image2, std::vector<GBL::Match_t>(0), GBL::KeyPointCollection_t(0), GBL::KeyPointCollection_t(0), outputFrame);
			}
			if(GBL::showStuff_b) {
				cv::imshow("tmp", outputFrame);
				cv::waitKey(10);
			}
			if(GBL::drawResults_b) {
				if(outputMethod != nullptr) {
					outputMethod->write(outputFrame);
				}
			}
			LOG_EXIT("Success");
			return GBL::RESULT_SUCCESS;
		}

		void Utils::printImage(const GBL::Image_t& image) {
			std::cout << "[" << std::endl;
			for(int32_t i = 0; i < image.rows; i++) {
				for(int32_t j = 0; j < image.cols; j++) {
					std::cout << std::hex << image.at<int>(i, j) << "\t";
				}
				std::cout << std::endl;
			}
			std::cout << "]" << std::endl;
		}
	}
}
