/*
 * =====================================================================================
 *
 *       Filename:  and.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/23/2014 12:13:51 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Bart Verhagen (bv), bart.verhagen@tass.be
 *   Organization:  TASS
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <chrono>

#include "cm/global.hpp"
#include "testUtils/measure.hpp"

#include "../and.hpp"

using EmbeddedMT::GBL::Frame_t;
using EmbeddedMT::Test::Measure::ObjectToMeasure;
using EmbeddedMT::Test::Measure::TimeMeasurement;
using EmbeddedMT::Test::Measure::getThousandUnit;

using EmbeddedMT::Test::Measure::useconds;
using EmbeddedMT::Test::Measure::nseconds;
using EmbeddedMT::Test::Measure::mseconds;

namespace {
	const uint8_t MASK_WIDTH_ONE_SIDE = 1U;
}

typedef EmbeddedMT::ImageProc::And SlowAnd;
typedef EmbeddedMT::ImageProc::Meta::AndSimd<uint8_t, uint32_t, MASK_WIDTH_ONE_SIDE> SimdAnd32;

namespace EmbeddedMT {
	namespace Test {
		namespace Performance {
			class MeasureAnd : public ObjectToMeasure  {
				public:
					virtual void operator()(void) const = 0;

				protected:
					MeasureAnd(const Frame_t& frame, const uint8_t maskWidthOneSide) :
						_maskWidthOneSide(maskWidthOneSide), _frame(frame)	{
						;
					}	
					const uint8_t _maskWidthOneSide;	
					Frame_t _frame;
			};

			class MeasureSimdAnd : public MeasureAnd  {
				public:
					MeasureSimdAnd(const Frame_t& frame, const uint8_t maskWidthOneSide) :
						MeasureAnd(frame, maskWidthOneSide)	{
						;
					}	
					virtual void operator()(void) const {
						SimdAnd32::fourNeighbourhood(_frame);	
					}	
			};

			class MeasureSlowAnd : public MeasureAnd  {
				public:
					MeasureSlowAnd(const Frame_t& frame, const uint8_t maskWidthOneSide) :
						MeasureAnd(frame, maskWidthOneSide)	{
						;
					}	
					virtual void operator()(void) const {
						SlowAnd::fourNeighbourhood(_frame, _maskWidthOneSide);	
					}	
			};

			template <typename duration>
			static void testAnds(const Frame_t& frame, const uint8_t maskWidthOneSide, const uint8_t nbOfMeasurements) {
				MeasureSimdAnd fastAnd(frame, maskWidthOneSide); 
				duration fastElapsed = TimeMeasurement<duration>::measure(fastAnd, nbOfMeasurements); 	

				LOG_MEASUREMENT_RESULT("Fast And - elapsed time: %f %s", fastElapsed.count()/1000.0, getThousandUnit(fastElapsed).c_str());

				MeasureSlowAnd slowAnd(frame, maskWidthOneSide);
				auto slowElapsed = TimeMeasurement<duration>::measure(slowAnd, nbOfMeasurements); 	
				LOG_MEASUREMENT_RESULT("Slow And - elapsed time: %f %s", slowElapsed.count()/1000.0, getThousandUnit(slowElapsed).c_str());
			}

			template<typename duration>
			static void measureAnd(const uint32_t rows, const uint32_t cols) {
				LOG_MEASUREMENT_START("MeasureAnd");
				const uint8_t nbOfMeasurements = 100U;
				const uint8_t maskWidthOneSide = MASK_WIDTH_ONE_SIDE;
				LOG_MEASUREMENT_DESCRIPTION("Number of measurements: %d", nbOfMeasurements);
				LOG_MEASUREMENT_DESCRIPTION("Mask width one side: %d", maskWidthOneSide);
				LOG_MEASUREMENT_DESCRIPTION("Image size: %d x %d", cols, rows); 

				// Fill random image
				Frame_t testImage = Frame_t(rows, cols, CV_8UC1);
				for(uint32_t row = 0; row < rows; ++row) {
					for(uint32_t col = 0; col < cols; ++col) {
						testImage.at<uint8_t>(row, col) = rand() % std::numeric_limits<uint8_t>::max();
					}
				}
				testAnds<duration>(testImage, maskWidthOneSide, nbOfMeasurements);
			}

			static void measureAnd(void) {
				srand(0);
				measureAnd<nseconds>(100, 100);
				measureAnd<useconds>(640, 480);
				measureAnd<useconds>(800, 1600);
				measureAnd<mseconds>(10000, 10000);
			}
		}
	}
}

int main(void) {
	EmbeddedMT::Test::Performance::measureAnd();
	return 0;
}

