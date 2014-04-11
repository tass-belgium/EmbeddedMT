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

std::vector<GBL::Displacement_t> findTheBall(GBL::ImageSequence_t& imageSequence, const ImageProc::ImageProc* imProc, const Draw::DrawInterface& drawer, const Descriptor::DescriptorInterface& descriptor, const Match::MatcherInterface& matcher, const Displacement::DisplacementInterface& displacement);

#endif /* FINDTHEBALL_HPP_ */
