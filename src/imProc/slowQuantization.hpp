#ifndef SLOWQUANTIZATION_HPP
#define SLOWQUANTIZATION_HPP

#include <iostream>
#include <assert.h>
#include <math.h>
#include <limits>

#include "cm/global.hpp"

namespace EmbeddedMT {
	namespace ImageProc {
		template <typename valueType>
		class SlowQuantization {
			public:
				static GBL::Frame_t quantizedBitExpansion(const GBL::Frame_t& frame, const uint8_t nbOfClasses) {
					assert(nbOfClasses > 0);
					assert(nbOfClasses <= GBL::NB_BITS_IN_A_BYTE * sizeof(valueType));

					GBL::Frame_t quantized(frame.rows, frame.cols, frame.type());
					const uint32_t interval = (std::numeric_limits<valueType>::max() - std::numeric_limits<valueType>::min() + 1) / nbOfClasses;
					for(uint16_t row = 0; row < frame.rows; ++row) {
						for(uint16_t col = 0; col < frame.cols; ++col) {
							uint8_t intervalIndex = 0;
							uint32_t intervalMaxValue = std::numeric_limits<valueType>::min() + interval;
							while(frame.at<valueType>(row, col) >= intervalMaxValue) {
								++intervalIndex;	
								intervalMaxValue += interval;
							}
							quantized.at<valueType>(row, col) = 0x01 << intervalIndex;
						}
					}
					return quantized;
				}

				static GBL::Frame_t unpackBitExpansion(GBL::Frame_t frame, const uint8_t nbOfClasses) {
					assert(nbOfClasses > 0);
					GBL::Frame_t unpacked = GBL::Frame_t::zeros(frame.rows,frame.cols, CV_8UC1);

					const uint32_t mappingFactor = (std::numeric_limits<valueType>::max() - std::numeric_limits<valueType>::min()) / nbOfClasses;
					for(int32_t row = 0; row < frame.rows; ++row) {
						for(int32_t col = 0; col < frame.cols; ++col) {
							unpacked.at<valueType>(row, col) = mappingFactor * log2(frame.at<valueType>(row, col));
						}
					}
					return unpacked;
				}

			private:
				static uint32_t log2(uint32_t value) {
					assert(value > 0);
					uint32_t result = -1;
					while(value > 0) {
						++result;
						value >>= 1;
					}
					return result;
				}
		};
	}
}
#endif
