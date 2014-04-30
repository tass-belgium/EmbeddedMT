/*
 * inputImageSequence.cpp
 *
 *  Created on: Apr 18, 2014
 *      Author: cv
 */
#include <dirent.h>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "log/logging.hpp"
#include "inputImageSequence.hpp"

namespace InputMethod {

bool InputImageSequence::isMoreInput() {
	return (_imageIndex < size());
}
GBL::CmRetCode_t InputImageSequence::start(const char* inputFile) {
	LOG_ENTER("Input file = %s", inputFile);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	if (setFiles(inputFile) != GBL::RESULT_SUCCESS) {
		LOG_ERROR("Could not retrieve content of %s", inputFile);
	} else {
		// Sort files
		std::sort(_images.begin() + 1, _images.end());
		_imageIndex = 0;
		result = GBL::RESULT_SUCCESS;
	}

	// Print file names
	for (uint32_t i = 0; i < _images.size(); i++) {
		LOG_INFO("Image %d of sequence is %s", i, _images[i].c_str());
	}
	LOG_EXIT("Result = %d", result);
	return result;
}

uint32_t InputImageSequence::size(void) const {
	return _images.size();
}

GBL::CmRetCode_t InputImageSequence::getFrame(uint16_t index, GBL::Frame_t& frame) {
	LOG_ENTER("Getting image %d in %p", index, &frame);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	if (index < _images.size()) {
		frame = cv::imread(_images[index], CV_LOAD_IMAGE_GRAYSCALE);
		result = GBL::RESULT_SUCCESS;
	} else {
		LOG_ERROR("There is no next frame");
	}
	LOG_EXIT("Result = %d", result);
	return result;
}

GBL::CmRetCode_t InputImageSequence::getNextFrame(GBL::Frame_t& nextFrame) {
	LOG_ENTER("nextFrame = %p, image index = %d", &nextFrame, _imageIndex);
	GBL::CmRetCode_t result = getFrame(_imageIndex, nextFrame);
	_imageIndex++;
	LOG_EXIT("Result = %d", result);
	return result;
}

GBL::CmRetCode_t InputImageSequence::stop(void) {
	return GBL::RESULT_SUCCESS;
}

GBL::CmRetCode_t InputImageSequence::setFiles(const char* const dir) {
	LOG_ENTER("Dir = %s", dir);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	char background[GBL::maxFilenameLength];
	snprintf(background, (size_t) GBL::maxFilenameLength, "%s/background.jpg", dir);
	_images.push_back(background);
	LOG_INFO("Used background image: %s", background);

	DIR *dp;
	struct dirent *dirp;
	dp = opendir(dir);
	if (dp == nullptr) {
		LOG_ERROR("Error opening %s", dir);
	} else {
		dirp = readdir(dp);
		while (dirp != nullptr) {
			if (isValidImage(dirp) == GBL::RESULT_SUCCESS) {
				char imagePath[GBL::maxFilenameLength];
				snprintf(imagePath, (size_t) GBL::maxFilenameLength, "%s/%s", dir, dirp->d_name);
				_images.push_back(imagePath);
			}
			dirp = readdir(dp);
			result = GBL::RESULT_SUCCESS;
		}
		closedir(dp);
	}
	LOG_EXIT("Result = %d", result);
	return result;
}

GBL::CmRetCode_t InputImageSequence::isValidImage(struct dirent* dirp) const {
	LOG_ENTER("File to check = %s", dirp->d_name);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	// Check if file
	if (dirp->d_type == DT_REG) {
		size_t len = strlen(dirp->d_name);
		// Check for length -> point to "." and ".."
		if (len > 2) {
			// We use filenames in stead of MIME-type for the moment
			if (std::strncmp(dirp->d_name, "background.jpg", sizeof("background.jpg")) != 0) {	// Check if it is the background image
				if (std::strncmp(&dirp->d_name[len - 4], ".jpg", sizeof(".jpg")) == 0) {
					result = GBL::RESULT_SUCCESS;
				}
			}
		}
	}
	LOG_EXIT("Result = %d", result);
	return result;
}

}
