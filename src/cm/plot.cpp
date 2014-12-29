/*
 * =====================================================================================
 *
 *       Filename:  plot.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/14/2014 02:38:12 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Bart Verhagen (bv), bart.verhagen@tass.be
 *   Organization:  TASS
 *
 * =====================================================================================
 */

#include <assert.h>

#include "plot.hpp"

using std::vector;
using cv::Vec3b;
using EmbeddedMT::GBL::Point;
using EmbeddedMT::GBL::Frame_t;
using EmbeddedMT::OutputMethod::OutputMethodInterface;

namespace EmbeddedMT {
	namespace Utils {
		std::vector<Vec3b> makeColorVector(void) {
			std::vector<Vec3b> colorVector(3);
			colorVector.push_back(Vec3b(0xFF, 0x0, 0x0));
			colorVector.push_back(Vec3b(0x0, 0xFF, 0x0));
			colorVector.push_back(Vec3b(0x0, 0x0, 0xFF));
			return colorVector;
		}
		const std::vector<Vec3b> Plot::colors = makeColorVector();

		void Plot::drawContours(const Frame_t& image, const vector<vector<Point> >& contours, OutputMethodInterface& outputMethod) {
			GBL::Frame_t contourImage;
			// Make sure there are 3 channels present
			cv::cvtColor(image, contourImage, cv::COLOR_GRAY2BGR);
			LOG_INFO("Contour image size=%dx%d, channels=%d", contourImage.rows, contourImage.cols, contourImage.channels());

			// Assert colors are available
			assert(colors.size() > 0);
	
			uint8_t i = 0;
			for(vector<vector<Point> >::const_iterator contourPoints = contours.begin(); contourPoints != contours.end(); ++contourPoints) {
				const Vec3b color = colors[i];
				for(vector<Point>::const_iterator contourPoint = contourPoints->begin(); contourPoint != contourPoints->end(); ++contourPoint) {
					contourImage.at<Vec3b>(contourPoint->getRow(), contourPoint->getCol()) = color;	
				}
				i = (i+1) % colors.size();
			}
			outputMethod.write(contourImage);
		}
	}
}
