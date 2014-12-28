/*
 * =====================================================================================
 *
 *       Filename:  quantization.cpp
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

#include "../quantization.hpp"
#include "../slowQuantization.hpp"

using EmbeddedMT::GBL::Frame_t;
using EmbeddedMT::ImageProc::Quantization;
using EmbeddedMT::ImageProc::SlowQuantization;
using EmbeddedMT::ImageProc::MetaQuantization;
using EmbeddedMT::ImageProc::VectorQuantization;
using EmbeddedMT::Test::Measure::ObjectToMeasure;
using EmbeddedMT::Test::Measure::TimeMeasurement;
using EmbeddedMT::Test::Measure::getThousandUnit;

using EmbeddedMT::Test::Measure::useconds;
using EmbeddedMT::Test::Measure::nseconds;
using EmbeddedMT::Test::Measure::mseconds;

namespace EmbeddedMT {
	namespace Test {
		namespace Performance {
			class MeasureQuantization : public ObjectToMeasure  {
				public:
					virtual void operator()(void) const = 0;

				protected:
					MeasureQuantization(const Frame_t& frame, const uint8_t nbOfClasses) :
						_nbOfClasses(nbOfClasses), _frame(frame)	{
						;
					}	
					const uint8_t _nbOfClasses;	
					Frame_t _frame;
			};

			class MeasureFastQuantization : public MeasureQuantization  {
				public:
					MeasureFastQuantization(const Frame_t& frame, const uint8_t nbOfClasses) :
						MeasureQuantization(frame, nbOfClasses)	{
						;
					}	
					virtual void operator()(void) const {
						Quantization<uint8_t>::quantizedBitExpansion(_frame, _nbOfClasses);	
					}	
			};

			class MeasureMetaQuantization : public MeasureQuantization  {
				public:
					MeasureMetaQuantization(const Frame_t& frame) :
						MeasureQuantization(frame, 0U)	{
						;
					}	
					virtual void operator()(void) const {
						MetaQuantization<uint8_t, 8>::quantizedBitExpansion(_frame);	
					}	
			};

			class MeasureVectorQuantization : public MeasureQuantization  {
				public:
					MeasureVectorQuantization(const Frame_t& frame) :
						MeasureQuantization(frame, 0U)	{
						;
					}	
					virtual void operator()(void) const {
						VectorQuantization<uint8_t, uint32_t, 8>::quantizedBitExpansion(_frame);	
					}	
			};


			class MeasureSlowQuantization : public MeasureQuantization  {
				public:
					MeasureSlowQuantization(const Frame_t& frame, const uint8_t nbOfClasses) :
						MeasureQuantization(frame, nbOfClasses)	{
						;
					}	
					virtual void operator()(void) const {
						SlowQuantization<uint8_t>::quantizedBitExpansion(_frame, _nbOfClasses);	
					}	
			};

			template <typename duration>
			static void testQuantizations(const Frame_t& frame, const uint8_t nbOfClasses, const uint8_t nbOfMeasurements) {
				MeasureFastQuantization fastQuant(frame, nbOfClasses); 
				duration fastElapsed = TimeMeasurement<duration>::measure(fastQuant, nbOfMeasurements); 	

				LOG_MEASUREMENT_RESULT("Fast quantization - elapsed time: %f %s", fastElapsed.count()/1000.0, getThousandUnit(fastElapsed).c_str());

				MeasureSlowQuantization slowQuant(frame, nbOfClasses);
				auto slowElapsed = TimeMeasurement<duration>::measure(slowQuant, nbOfMeasurements); 	
				LOG_MEASUREMENT_RESULT("Slow quantization - elapsed time: %f %s", slowElapsed.count()/1000.0, getThousandUnit(slowElapsed).c_str());
				
				MeasureMetaQuantization metaQuant(frame);
				auto metaElapsed = TimeMeasurement<duration>::measure(metaQuant, nbOfMeasurements); 	
				LOG_MEASUREMENT_RESULT("Meta quantization - elapsed time: %f %s", metaElapsed.count()/1000.0, getThousandUnit(metaElapsed).c_str());

				MeasureVectorQuantization vectorQuant(frame);
				auto vectorElapsed = TimeMeasurement<duration>::measure(vectorQuant, nbOfMeasurements); 	
				LOG_MEASUREMENT_RESULT("Vector quantization - elapsed time: %f %s", vectorElapsed.count()/1000.0, getThousandUnit(vectorElapsed).c_str());
			}

			template<typename duration>
			static void measureQuantization1(const uint32_t rows, const uint32_t cols) {
				LOG_MEASUREMENT_START("MeasureQuantization1");
				const uint8_t nbOfMeasurements = 100U;
				const uint8_t nbOfClasses = 8U;
				LOG_MEASUREMENT_DESCRIPTION("Number of measurements: %d", nbOfMeasurements);
				LOG_MEASUREMENT_DESCRIPTION("Number of quantization classes: %d", nbOfClasses);
				LOG_MEASUREMENT_DESCRIPTION("Image size: %d x %d", cols, rows); 

				// Fill random image
				Frame_t testImage = Frame_t(rows, cols, CV_8UC1);
				for(uint32_t row = 0; row < rows; ++row) {
					for(uint32_t col = 0; col < cols; ++col) {
						testImage.at<uint8_t>(row, col) = rand() % std::numeric_limits<uint8_t>::max();
					}
				}
				testQuantizations<duration>(testImage, nbOfClasses, nbOfMeasurements);
			}

			static void measureQuantization(void) {
				srand(0);
				measureQuantization1<nseconds>(100, 100);
				measureQuantization1<useconds>(800, 1600);
				measureQuantization1<mseconds>(10000, 10000);
			}
		}
	}
}

int main(void) {
	EmbeddedMT::Test::Performance::measureQuantization();
	return 0;
}

