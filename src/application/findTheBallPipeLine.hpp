/*
 * findTheBallPipeLine.hpp
 *
 *  Created on: Apr 28, 2014
 *      Author: Bart Verhagen
 */

#ifndef FINDTHEBALLPIPELINE_HPP_
#define FINDTHEBALLPIPELINE_HPP_

#include "cm/global.hpp"
#include "imProc/imageProcInterface.hpp"
#include "descriptor/descriptorInterface.hpp"
#include "match/matchInterface.hpp"
#include "draw/drawInterface.hpp"
#include "displacement/displacementInterface.hpp"
#include "inputMethod/inputMethodInterface.hpp"
#include "outputMethod/outputMethodInterface.hpp"

std::vector<GBL::Displacement_t> findTheBallPipeline(const char* const videoFile, const ImageProc::ImageProc* imProc, Draw::DrawInterface& drawer,
		const Descriptor::DescriptorInterface& descriptorInterface, const Match::MatcherInterface& matcherInterface, const Displacement::DisplacementInterface& displacementInterface,
		InputMethod::InputMethodInterface& inputMethodInterface, OutputMethod::OutputMethodInterface& outputMethodInterface);

#endif
