/*
 * global.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */

#ifndef GLOBAL_HPP_
#define GLOBAL_HPP_

#include <string>
#include <stdint.h>
#include <vector>
#include <cv.h>

#define nullptr 0

typedef char char_t;
typedef float float_t;
typedef bool bool_t;

namespace GBL {

typedef cv::Mat Image_t;
typedef const cv::InputArray InputImage_t;
typedef cv::OutputArray OutputImage_t;

typedef cv::Mat Frame_t;

typedef cv::KeyPoint KeyPoint_t;
typedef std::vector<KeyPoint_t> KeyPointCollection_t;

typedef cv::Mat Descriptor_t;

typedef cv::DMatch Match_t;
typedef std::vector<Match_t> MatchCollection_t;

typedef enum cmRetCodes {
	RESULT_SUCCESS,
	RESULT_FAILURE
} CmRetCode_t;

typedef struct point {
	uint32_t x;
	uint32_t y;
} Point_t;

typedef struct displacement {
	int32_t x;
	int32_t y;
} Displacement_t;

const uint16_t maxFilenameLength = 256;

}

#endif /* GLOBAL_HPP_ */
