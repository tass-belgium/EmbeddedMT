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

std::vector<GBL::Displacement_t> findTheBall(const char* const videoFile, const ImageProc::ImageProc* imProc, Draw::DrawInterface& drawer, const Descriptor::DescriptorInterface& descriptor, const Match::MatcherInterface& matcher, const Displacement::DisplacementInterface& displacement, InputMethod::InputMethodInterface& inputMethod, OutputMethod::OutputMethodInterface& outputMethod);

#endif /* FINDTHEBALL_HPP_ */
