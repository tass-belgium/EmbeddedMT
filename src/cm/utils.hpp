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

namespace embeddedMT {
	namespace Utils {

		typedef void (*DrawResultProc_f)(const imageProc::ImageProc&, const GBL::Image_t&, const GBL::Image_t&, GBL::Image_t&);

		class Utils {
		public:

			// Deprecated
			static GBL::CmRetCode_t drawResults (InputMethod::InputMethodInterface& inputMethod, Draw::DrawInterface& drawer, const imageProc::ImageProc& imProc, std::vector<GBL::DescriptorContainer_t> descriptors, std::vector<GBL::MatchesContainer_t> allMatches, uint32_t nbFrames, const GBL::Image_t& image2);
			// Deprecated
			static GBL::CmRetCode_t drawHelper(OutputMethod::OutputMethodInterface& outputMethod, InputMethod::InputMethodInterface& inputMethod, Draw::DrawInterface& drawer, const imageProc::ImageProc& imProc, std::vector<GBL::DescriptorContainer_t> descriptors, std::vector<GBL::MatchesContainer_t> allMatches, uint32_t nbFrames, const char* outputFile, DrawResultProc_f procFunction, const GBL::Image_t* const image2);
			static GBL::CmRetCode_t drawResult(const GBL::Frame_t image1, const GBL::Frame_t image2, const Draw::DrawInterface& drawer, GBL::DescriptorContainer_t descriptor1, GBL::DescriptorContainer_t descriptor2, GBL::MatchesContainer_t matches, OutputMethod::OutputMethodInterface* outputMethod);	
			static void printImage(const GBL::Image_t& image);

		private:
			static void fastSubtractHandler(const imageProc::ImageProc& imProc, const GBL::Image_t& image1, const GBL::Image_t& image2, GBL::Image_t& outputImage);
		};

		template<class T> class Unused {
			public:
				static inline void UnusedVariable(T unusedVariable) {
					(void) unusedVariable;
				}
			private:
				Unused();
		};

	}
}
#endif
