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
#include "detector/detectorInterface.hpp"
#include "descriptor/descriptorInterface.hpp"
#include "match/matchInterface.hpp"
#include "draw/drawInterface.hpp"
#include "displacement/displacementInterface.hpp"
#include "inputMethod/inputMethodInterface.hpp"
#include "outputMethod/outputMethodInterface.hpp"

std::vector<EmbeddedMT::GBL::Displacement_t> findTheBallPipeline(const char* const videoFile, const EmbeddedMT::ImageProc::ImageProc* imProc, EmbeddedMT::Draw::DrawInterface& drawer,
		const EmbeddedMT::Detector::DetectorInterface& detectorInterface, const EmbeddedMT::Descriptor::DescriptorInterface& descriptorInterface, const EmbeddedMT::Match::MatcherInterface& matcherInterface, const EmbeddedMT::Displacement::DisplacementInterface& displacementInterface, EmbeddedMT::InputMethod::InputMethodInterface& inputMethodInterface, EmbeddedMT::OutputMethod::OutputMethodInterface& outputMethodInterface);

#endif
