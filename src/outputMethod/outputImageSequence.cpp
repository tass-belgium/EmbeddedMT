/*
 * outputImageSequence.cpp
 *
 *  Created on: Apr 18, 2014
 *      Author: cv
 */
#include <sys/stat.h>
#include <sys/types.h>
#include <sstream>
#include <iomanip>

#include "log/logging.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"

#include "outputImageSequence.hpp"

using std::setfill;
using std::setw;
using std::stringstream;

namespace EmbeddedMT {
	namespace OutputMethod {
		OutputImageSequence::OutputImageSequence(const std::string& filename) {
			LOG_ENTER("Filename = %s", filename.c_str());
			_imageIndex = 0;

			std::string outputFolderName("output");
			stringstream stream;
			stream << outputFolderName << filename;
			_filename = stream.str();
			struct stat sb;

			if (stat(outputFolderName.c_str(), &sb) != 0) {
				// Try creating it
				if(mkdir(outputFolderName.c_str(), S_IRWXU) != 0) {
					LOG_ERROR("Could not create directory %s", outputFolderName.c_str());
				}		
			} else {
				if(! S_ISDIR(sb.st_mode)) {
					LOG_ERROR("%s is not a directory", outputFolderName.c_str());
				}
			}
		}

		GBL::CmRetCode_t OutputImageSequence::write(const GBL::Frame_t& frame) {
			LOG_ENTER("Frame = %p", &frame);
			GBL::CmRetCode_t result = GBL::RESULT_FAILURE;

			stringstream stream;
			stream << _filename << "-" << setfill('0') << setw(3) << _imageIndex << ".jpg";
			std::string tmpFilename = stream.str();
			if(cv::imwrite(tmpFilename, frame) == true) {
				_imageIndex++;
				result = GBL::RESULT_SUCCESS;
			}
			LOG_EXIT("result = %d", result);
			return result;
		}
	}
}
