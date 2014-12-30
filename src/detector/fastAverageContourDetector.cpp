/*
 * =====================================================================================
 *
 *       Filename:  averageContourDetector.cpp
 *
 *    Description:  Detector that calculates the middlepoint of the detected contours
 *
 *        Version:  1.0
 *        Created:  05/06/2014 11:04:10 AM
 *       Revision:  none
 *
 *         Author:  Bart Verhagen (bv), bart.verhagen@tass.be
 *   Organization:  TASS
 *
 * =====================================================================================
 */
#include "fastAverageContourDetector.hpp"
#include "imProc/regionGrowing/findContours.hpp"
#include "imProc/quantization.hpp"

#include "cm/plot.hpp"
#include "outputMethod/outputImageSequence.hpp"
#include "opencv2/highgui/highgui.hpp"

using EmbeddedMT::ImageProc::Contours;
using EmbeddedMT::ImageProc::VectorQuantization;
using EmbeddedMT::OutputMethod::OutputImageSequence;

namespace EmbeddedMT {
	namespace Detector {
		FastAverageContourDetector::FastAverageContourDetector(const region_t minimumRegionSize, const uint8_t maskWidthOneSide) 
			: _minimumRegionSize(minimumRegionSize), _maskWidthOneSide(maskWidthOneSide) {
			;
		}

		GBL::CmRetCode_t FastAverageContourDetector::detect(const GBL::Image_t& inputImage, GBL::KeyPointCollection_t& detectedKeypoints) const {
			LOG_ENTER("Image = %p, keypoints = %p", &inputImage, &detectedKeypoints);
			GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
			LOG_EXIT("result = %d", result);
			return result;
		}

		GBL::CmRetCode_t FastAverageContourDetector::detect(const GBL::Image_t& inputImage, GBL::KeyPointCollection_t& detectedKeypoints, const GBL::Image_t& background, const ImageProc::ImageProc& imProc) const {
			GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
			GBL::Image_t outputImage = inputImage - background;

			// Quantize picture
			outputImage = VectorQuantization<uint8_t, uint32_t, 4>::quantizedBitExpansion(outputImage); 

			Contours findContours(_minimumRegionSize, _maskWidthOneSide);
			std::vector<std::vector<GBL::Point> > contours = findContours.find(outputImage);

			LOG_INFO("Found %d contours", (uint32_t) contours.size());
			for(size_t i = 0; i < contours.size(); i++) {
				uint32_t x = 0;
				uint32_t y = 0;
				for(size_t j = 0; j < contours[i].size(); j++) {
					x += (contours[i])[j].getX();
					y += (contours[i])[j].getY();		
				}
				float_t x_center = (float_t) x/ (float_t) contours[i].size();
				float_t y_center = (float_t) y/ (float_t) contours[i].size();
				// TODO: determine scale and angle for more robust keypoints descriptions
				GBL::KeyPoint_t keypoint(x_center, y_center, 1); 
				detectedKeypoints.push_back(keypoint);
			}
			result = GBL::RESULT_SUCCESS;
			return detect(outputImage, detectedKeypoints);
		}
	}
}
