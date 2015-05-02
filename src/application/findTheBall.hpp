/*
 * findTheBall.hpp
 *
 *  Created on: Apr 8, 2014
 *      Author: cv
 */

#ifndef FINDTHEBALL_HPP_
#define FINDTHEBALL_HPP_

#include "cm/global.hpp"
#include "imProc/imageProcInterface.hpp"
#include "descriptor/descriptorInterface.hpp"
#include "match/matchInterface.hpp"
#include "draw/drawInterface.hpp"
#include "displacement/displacementInterface.hpp"
#include "inputMethod/inputMethodInterface.hpp"
#include "outputMethod/outputMethodInterface.hpp"

std::vector<EmbeddedMT::GBL::Displacement_t> findTheBall(const char* const videoFile, const EmbeddedMT::ImageProc::ImageProc* imProc, EmbeddedMT::Draw::DrawInterface& drawer, const EmbeddedMT::Descriptor::DescriptorInterface& descriptor, const EmbeddedMT::Match::MatcherInterface& matcher, const EmbeddedMT::Displacement::DisplacementInterface& displacement, EmbeddedMT::InputMethod::InputMethodInterface& inputMethod, EmbeddedMT::OutputMethod::OutputMethodDisplacementInterface& outputMethod);

#endif /* FINDTHEBALL_HPP_ */
