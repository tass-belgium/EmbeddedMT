/*
 * =====================================================================================
 *
 *       Filename:  profileQuantization.cpp
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

#include "imProc/quantization.hpp"
#include "imProc/slowQuantization.hpp"

using EmbeddedMT::GBL::Frame_t;

using EmbeddedMT::ImageProc::SlowQuantization;
using EmbeddedMT::ImageProc::Quantization;
using EmbeddedMT::ImageProc::MetaQuantization;

enum QuantizationMethod {
	SLOW_QUANTIZATION,
	FAST_QUANTIZATION,
	META_QUANTIZATION,
	QUANTIZATION_END
};

static std::map<QuantizationMethod, std::string> quantizationMap;

static void help(std::string progName) {
	LOG_HELP("Usage: " << progName << " quantizationMethod");
	LOG_HELP("Where:");
	LOG_HELP("\tquantizationMethod...\tThe quantization method to use:");
	for(uint32_t i = 0; i < QUANTIZATION_END; ++i) {
		LOG_HELP("\t\t\t\t\t" << quantizationMap[static_cast<QuantizationMethod>(i)] << " = " << i);
	}	
}

static void occupyMap(void) {
	quantizationMap.insert(std::pair<QuantizationMethod, std::string>(SLOW_QUANTIZATION, "Slow Quantization"));
	quantizationMap.insert(std::pair<QuantizationMethod, std::string>(FAST_QUANTIZATION, "Fast Quantization"));
	quantizationMap.insert(std::pair<QuantizationMethod, std::string>(META_QUANTIZATION, "Meta Quantization"));	
}

static void slowQuantizationWrapper(const Frame_t& image, const uint32_t nbOfClasses) {
	SlowQuantization<uint8_t>::quantizedBitExpansion(image, nbOfClasses);	
}

static void fastQuantizationWrapper(const Frame_t& image, const uint32_t nbOfClasses) {
	Quantization<uint8_t>::quantizedBitExpansion(image, nbOfClasses);
}

template <unsigned nbOfClasses>
static void metaQuantizationWrapper(const Frame_t& image) {
	MetaQuantization<uint8_t, nbOfClasses>::quantizedBitExpansion(image);
}

static void executeQuant(QuantizationMethod method) {
	const uint32_t rows = 10000;
	const uint32_t cols = 10000;
	const uint8_t nbOfClasses = 8U;
	// Fill random image
	Frame_t testImage = Frame_t(rows, cols, CV_8UC1);
	for(uint32_t row = 0; row < rows; ++row) {
		for(uint32_t col = 0; col < cols; ++col) {
			testImage.at<uint8_t>(row, col) = rand() % std::numeric_limits<uint8_t>::max();
		}
	}

	for(uint32_t i = 0; i < 10; ++i) {
		switch(method) {
			case SLOW_QUANTIZATION:
				slowQuantizationWrapper(testImage, nbOfClasses);
				break;
			case FAST_QUANTIZATION:
				fastQuantizationWrapper(testImage, nbOfClasses);
				break;
			case META_QUANTIZATION:
				metaQuantizationWrapper<nbOfClasses>(testImage);
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
		QuantizationMethod type = static_cast<QuantizationMethod>(strtol(argv[i], 0, 10));
		LOG_INFO("Executing for %s", quantizationMap[type].c_str());
		executeQuant(type);
	}
	return 0;
}
