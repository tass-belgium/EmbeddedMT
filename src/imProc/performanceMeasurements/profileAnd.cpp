/*
 * =====================================================================================
 *
 *       Filename:  profileAnd.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/24/2014 11:55:13 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Bart Verhagen (bv), bart.verhagen@tass.be
 *   Organization:  TASS
 *
 * =====================================================================================
 */
#include <string>
#include <map>
#include <random>

#include "opencv2/core/core.hpp"

#include "log/logging.hpp"
#include "cm/global.hpp"

#include "imProc/and.hpp"

using EmbeddedMT::GBL::Frame_t;

typedef EmbeddedMT::ImageProc::And ReferenceAnd;

enum AndMethod {
	REFERENCE_AND,
	SIMD_AND,
	AND_END
};

static std::map<AndMethod, std::string> andMap;

static void help(std::string progName) {
	LOG_HELP("Usage: " << progName << " andMethod");
	LOG_HELP("Where:");
	LOG_HELP("\tandMethod...\tThe and method to use:");
	for(uint32_t i = 0; i < AND_END; ++i) {
		LOG_HELP("\t\t\t\t\t" << andMap[static_cast<AndMethod>(i)] << " = " << i);
	}	
}

static void occupyMap(void) {
	andMap.insert(std::pair<AndMethod, std::string>(REFERENCE_AND, "Reference And"));
	andMap.insert(std::pair<AndMethod, std::string>(SIMD_AND, "Simd And"));
}

static void referenceAndWrapper(const Frame_t& image, const uint32_t maskWidthOneSide) {
	ReferenceAnd::fourNeighbourhood(image, maskWidthOneSide);	
}

template <unsigned maskWidthOneSide>
static void simdAndWrapper(const Frame_t& image) {
	EmbeddedMT::ImageProc::Meta::AndSimd<uint8_t, uint32_t, maskWidthOneSide>::fourNeighbourhood(image);
}

static void executeAnd(AndMethod method) {
	const uint32_t rows = 10000;
	const uint32_t cols = 10000;
	const uint8_t maskWidthOneSide = 1U;
	// Fill random image
	Frame_t testImage = Frame_t(rows, cols, CV_8UC1);
	for(uint32_t row = 0; row < rows; ++row) {
		for(uint32_t col = 0; col < cols; ++col) {
			testImage.at<uint8_t>(row, col) = rand() % std::numeric_limits<uint8_t>::max();
		}
	}

	for(uint32_t i = 0; i < 10; ++i) {
		switch(method) {
			case REFERENCE_AND:
				referenceAndWrapper(testImage, maskWidthOneSide);
				break;
			case SIMD_AND:
				simdAndWrapper<maskWidthOneSide>(testImage);
				break;
			default:
				LOG_ERROR("Method not handled");
		}	
	}
}

int main(int argc, char** argv) {
	srand(0);
	occupyMap();
	if(argc < 2) {
		LOG_ERROR("You should provide at least one argument");
		help(std::string(argv[0]));
		return 1;
	}

	for(int32_t i = 1; i < argc; ++i) {
		AndMethod type = static_cast<AndMethod>(strtol(argv[i], 0, 10));
		LOG_INFO("Executing for %s", andMap[type].c_str());
		executeAnd(type);
	}
	return 0;
}
