/*
 * utils.hpp
 *
 *  Created on: Apr 28, 2014
 *      Author: Bart Verhagen
 */

#ifndef UTILS_HPP_
#define UTILS_HPP_

#include "cm/global.hpp"
#include "imProc/imageProcInterface.hpp"
#include "draw/drawInterface.hpp"
#include "inputMethod/inputMethodInterface.hpp"
#include "outputMethod/outputMethodInterface.hpp"

namespace Utils {

typedef void (*DrawResultProc_f)(const ImageProc::ImageProc&, const GBL::Image_t&, const GBL::Image_t&, GBL::Image_t&);

class Utils {
public:
	static GBL::CmRetCode_t drawResults (InputMethod::InputMethodInterface& inputMethod, Draw::DrawInterface& drawer, const ImageProc::ImageProc& imProc, GBL::DescriptorContainer_t* descriptors, GBL::MatchesContainer_t* allMatches, uint32_t nbFrames, const GBL::Image_t& image2);
	static GBL::CmRetCode_t drawHelper(OutputMethod::OutputMethodInterface& outputMethod, InputMethod::InputMethodInterface& inputMethod, Draw::DrawInterface& drawer, const ImageProc::ImageProc& imProc, GBL::DescriptorContainer_t* descriptors, GBL::MatchesContainer_t* allMatches, uint32_t nbFrames, const char* outputFile, DrawResultProc_f procFunction, const GBL::Image_t* const image2);

private:
	static void fastSubtractHandler(const ImageProc::ImageProc& imProc, const GBL::Image_t& image1, const GBL::Image_t& image2, GBL::Image_t& outputImage);
};

}

#endif
