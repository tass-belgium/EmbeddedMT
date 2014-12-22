/*
 * outputImageSequence.cpp
 *
 *  Created on: Apr 18, 2014
 *      Author: cv
 */
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#include "log/logging.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"

#include "outputImageSequence.hpp"

namespace EmbeddedMT {
	namespace OutputMethod {
		GBL::CmRetCode_t OutputImageSequence::open(const char* filename) {
			LOG_ENTER("Filename = %s", filename);
			GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
			_imageIndex = 0;
			const char* outputFolderName = "output";
			snprintf(_filename, GBL::maxFilenameLength, "%s/%s", outputFolderName, filename);
			struct stat sb;

			if (stat(outputFolderName, &sb) != 0) {
				// Try creating it
				if(mkdir(outputFolderName, S_IRWXU) == 0) {
					result = GBL::RESULT_SUCCESS;
				} else {
					LOG_ERROR("Could not create directory %s", outputFolderName);
				}		
			} else {
				if(S_ISDIR(sb.st_mode)) {
					result = GBL::RESULT_SUCCESS;
				} else {
					LOG_ERROR("%s is not a directory", outputFolderName);
				}
			}
			LOG_EXIT("result = %d", result);
			return result;
		}

		GBL::CmRetCode_t OutputImageSequence::write(const GBL::Frame_t frame) {
			LOG_ENTER("Frame = %p", &frame);
			GBL::CmRetCode_t result = GBL::RESULT_FAILURE;

			char tmpFilename[GBL::maxFilenameLength];
			snprintf(tmpFilename, GBL::maxFilenameLength, "%s-%03d.jpg", _filename, _imageIndex);
			if(cv::imwrite(tmpFilename, frame) == true) {
				_imageIndex++;
				result = GBL::RESULT_SUCCESS;
			}
			LOG_EXIT("result = %d", result);
			return result;
		}

		GBL::CmRetCode_t OutputImageSequence::write(const GBL::Displacement_t& displacement) {
			return GBL::RESULT_FAILURE;
		}

		GBL::CmRetCode_t OutputImageSequence::close() {
			return GBL::RESULT_SUCCESS;
		}
	}
}
