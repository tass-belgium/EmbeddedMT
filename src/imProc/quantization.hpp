#ifndef QUANTIZATION_HPP
#define QUANTIZATION_HPP

#include <assert.h>
#include "cm/global.hpp"
#include "cm/meta.hpp"

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
		
		template <typename ValueType, int nbOfClasses>
		class MetaQuantization {
		  public:
			  // Seems like we are counting on copy elision on this one
			static GBL::Frame_t quantizedBitExpansion(const GBL::Frame_t& frame) {
				assert(nbOfClasses > 0);
				assert(nbOfClasses <= GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType));

				const ValueType shifter = GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType) - Utils::Meta::Log2<nbOfClasses>::result;
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

		template <typename ValueType>
		class MetaQuantization<ValueType, 1> {
			public:
				// Just return all ones
				static GBL::Frame_t quantizedBitExpansion(const GBL::Frame_t& frame) {
					return GBL::Frame_t::ones(frame.rows, frame.cols, frame.type());
				}
		};

		template <typename ValueType>
		class MetaQuantization<ValueType, 0> {
			public:
				// Just return all ones
				static GBL::Frame_t quantizedBitExpansion(const GBL::Frame_t& frame) {
					assert(false);
					return GBL::Frame_t(0, 0, frame.type());
				}
		};


		template <typename ValueType, typename VectorType, int nbOfClasses>
		class VectorQuantization {
		  private:
			union VectorConversion {
				VectorType vector;
				ValueType values[sizeof(VectorType)/sizeof(ValueType)];
			};
		  public:
			  // Seems like we are counting on copy elision on this one
			static GBL::Frame_t quantizedBitExpansion(const GBL::Frame_t& frame) {
				STATIC_ASSERT(nbOfClasses > 0, "nbOfClasses should be bigger than 0");
				STATIC_ASSERT(nbOfClasses <= GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType), "nbOfClasses should be smaller than the number of bits of in the ValueType");

				const ValueType shifter = GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType) - Utils::Meta::Log2<nbOfClasses>::result;
				// Make sure that the ValueType fits an integer number of times in the VectorType
				STATIC_ASSERT(sizeof(VectorType) % sizeof(ValueType) == 0, "Size of the VectorType should be an integer multiple of the ValueType");
				const ValueType valueTypesPerVector = sizeof(VectorType)/sizeof(ValueType);

				// The shiftmask should zero the bits that were shifted from a neighbouring ValueType value, therefore only the 'significant' bits should be 1.
				// The shiftmask exists out of the value log2(nbOfClasses) repeated valueTypesPerVector times
				const ValueType mask = Utils::Meta::AllOnesUntil<Utils::Meta::Log2<nbOfClasses>::result>::value; // Mask should be all 1's until the log2<nbOfClasses> bit

				const VectorType shiftMask = Utils::Meta::RepeatTypedX<ValueType, VectorType, mask, valueTypesPerVector - 1>::value;

				GBL::Frame_t quantized(frame.rows, frame.cols, frame.type());

				for(uint16_t row = 0; row < frame.rows; ++row) {
					// Reintepret data pointers to be pointing to VectorTypes instead of Valuetypes
					const VectorType* srcRowPtr = reinterpret_cast<const VectorType*>(frame.ptr(row));
					VectorType* dstRowPtr = reinterpret_cast<VectorType*>(quantized.ptr(row));

					for(uint16_t col = 0; col < frame.cols/valueTypesPerVector; ++col) {
						// Get the value to shift for. This depends on the n most significant bits.
						const VectorType valueToShift = (*srcRowPtr >> shifter) & shiftMask;

						// Would be nice if we could vectorize this further
						VectorConversion tmpVector;
					    tmpVector.vector = valueToShift;
						VectorConversion result;
						for(uint8_t i = 0; i < valueTypesPerVector; ++i) {
							assert(tmpVector.values[i] < GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType));
							result.values[i] = 0x01 << tmpVector.values[i];	
						}

						// Shift a bit to the appropriate place
						*dstRowPtr = result.vector;

						// Shift to the next value
						++srcRowPtr;
						++dstRowPtr;
					}

					// Handle the leftovers at the side that did not fit into VectorType
					const ValueType* srcRowPtrSide = reinterpret_cast<const ValueType*>(srcRowPtr);
					ValueType* dstRowPtrSide = reinterpret_cast<ValueType*>(dstRowPtr);
					for(uint16_t col = 0; col < frame.cols % valueTypesPerVector; ++col) {
						const ValueType valueToShift = *srcRowPtrSide >> shifter;
						assert(valueToShift < GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType));

						*dstRowPtrSide = 0x01 << valueToShift;

						++srcRowPtrSide;
						++dstRowPtrSide;
					}
				}
				return quantized;
			}
		};

		template <typename ValueType, typename VectorType>
		class VectorQuantization<ValueType, VectorType, 1> {
			public:
				// Just return all ones
				static GBL::Frame_t quantizedBitExpansion(const GBL::Frame_t& frame) {
					return GBL::Frame_t::ones(frame.rows, frame.cols, frame.type());
				}
		};
	}
}

#endif
