/*
 * testInputVideo.cpp
 *
 *  Created on: Apr 18, 2014
 *      Author: cv
 */

#include "cm/global.hpp"
#include "../inputVideo.hpp"

int main(int argc, char** argv) {
	InputMethod::InputVideo inputVideo;

	inputVideo.start("../../../../../testSequences/tennisball_video1/tennisball_video1_DVD.dvd");
	for(uint32_t i = 0; i < inputVideo.size(); i++) {
		GBL::Frame_t frame;
		inputVideo.getNextFrame(frame);

		cv::imshow("Videotest", frame);
		cv::waitKey(500);
	}

	for(uint32_t i = 0; i < inputVideo.size(); i++) {
		GBL::Frame_t frame;
		inputVideo.getFrame(i, frame);

		cv::imshow("Videotest2", frame);
		cv::waitKey(500);
	}
	return 0;
}


