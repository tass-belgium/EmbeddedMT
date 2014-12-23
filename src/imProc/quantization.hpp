#ifndef QUANTIZATION_HPP
#define QUANTIZATION_HPP

#include <assert.h>
#include "cm/global.hpp"

namespace EmbeddedMT {
	namespace ImageProc {
		template <typename ValueType>
		class Quantization {
		  public:
			  // Seems like we are counting on copy elision on this one
			static GBL::Frame_t quantizedBitExpansion(const GBL::Frame_t& frame, const uint8_t nbOfClasses) {
				assert(nbOfClasses > 0);
				assert(nbOfClasses <= GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType));

				const ValueType shifter = GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType) - log2(nbOfClasses);
				GBL::Frame_t quantized(frame.rows, frame.cols, frame.type());

				for(uint16_t row = 0; row < frame.rows; ++row) {
					const ValueType* srcRowPtr = frame.ptr(row);
					ValueType* dstRowPtr = quantized.ptr(row);

					for(uint16_t col = 0; col < frame.cols; ++col) {
						// Get the value to shift for. This depends on the n most significant bits.
						const ValueType valueToShift = *srcRowPtr >> shifter;
						assert(valueToShift < GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType));

						// Shift a bit to the appropriate place
						*dstRowPtr = 0x01 << valueToShift;

						// Shift to the next value
						++srcRowPtr;
						++dstRowPtr;
					}
				}
				return quantized;
			}

		  private:
			static inline uint32_t log2(uint32_t value) {
				int32_t result = -1;
				while (value != 0) {
					++result;
					value >>= 1;
				}
				return static_cast<uint32_t>(result);
			}
		};
		
		template <unsigned value>
		struct Log2 {
			// Compiler gets confused with the shift operator for the division by two. Since this is compile time, we can just divide by two
			enum { result = (1 + Log2<value/2>::result) };
		};

		template <>
		struct Log2<0> {
			enum { result = -1 };
		};

		template <typename ValueType, int nbOfClasses>
		class MetaQuantization {
		  public:
			  // Seems like we are counting on copy elision on this one
			static GBL::Frame_t quantizedBitExpansion(const GBL::Frame_t& frame) {
				assert(nbOfClasses > 0);
				assert(nbOfClasses <= GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType));

				const ValueType shifter = GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType) - Log2<nbOfClasses>::result;
				GBL::Frame_t quantized(frame.rows, frame.cols, frame.type());

				for(uint16_t row = 0; row < frame.rows; ++row) {
					const ValueType* srcRowPtr = frame.ptr(row);
					ValueType* dstRowPtr = quantized.ptr(row);

					for(uint16_t col = 0; col < frame.cols; ++col) {
						// Get the value to shift for. This depends on the n most significant bits.
						const ValueType valueToShift = *srcRowPtr >> shifter;
						assert(valueToShift < GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType));

						// Shift a bit to the appropriate place
						*dstRowPtr = 0x01 << valueToShift;

						// Shift to the next value
						++srcRowPtr;
						++dstRowPtr;
					}
				}
				return quantized;
			}
		};
	}
}

#endif
