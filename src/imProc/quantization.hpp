#ifndef QUANTIZATION_HPP
#define QUANTIZATION_HPP

#include <assert.h>
#include "cm/global.hpp"
#include "cm/meta.hpp"
#include "cm/mtMath.hpp"
#include "vector/shift.hpp"

namespace EmbeddedMT {
	namespace ImageProc {
		template <typename ValueType>
		class Quantization {
		  public:
			static GBL::Frame_t quantizedBitExpansion(const GBL::Frame_t& frame, const uint8_t nbOfClasses);
			static inline ValueType quantizedBitExpansion(const ValueType value, const ValueType shifter);
		};

		
		template <typename ValueType, unsigned nbOfClasses>
		class MetaQuantization {
		  public:
			static GBL::Frame_t quantizedBitExpansion(const GBL::Frame_t& frame);
		};

		// MetaQuantization partial specializations for corner cases
		template <typename ValueType>
		class MetaQuantization<ValueType, 1U> {
			public:
				static GBL::Frame_t quantizedBitExpansion(const GBL::Frame_t& frame);
		};

		template <typename ValueType, typename VectorType, unsigned nbOfClasses>
		class VectorQuantization {
		  private:
				union VectorConversion {
					VectorType vector;
					ValueType values[sizeof(VectorType)/sizeof(ValueType)];
				};
		  public:
			static GBL::Frame_t quantizedBitExpansion(const GBL::Frame_t& frame);
			static inline VectorType quantizedBitExpansion(const VectorType value);
		};

		// Vector quantization partial specialization
		template <typename ValueType, typename VectorType>
		class VectorQuantization<ValueType, VectorType, 1U> {
			public:
				static GBL::Frame_t quantizedBitExpansion(const GBL::Frame_t& frame);
		};




		// Template function definitions
		template <typename ValueType>
		ValueType Quantization<ValueType>::quantizedBitExpansion(const ValueType value, const ValueType shifter) {
			assert(shifter <= sizeof(ValueType) * GBL::NB_BITS_IN_A_BYTE);
			const ValueType valueToShift = value >> shifter;
			assert(valueToShift < GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType));

			// Shift a bit to the appropriate place
			return 0x01 << valueToShift;
		}

		template <typename ValueType>
		GBL::Frame_t Quantization<ValueType>::quantizedBitExpansion(const GBL::Frame_t& frame, const uint8_t nbOfClasses) {
			assert(nbOfClasses > 0);
			assert(nbOfClasses <= GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType));

			const ValueType shifter = GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType) - Math::Math::log2(nbOfClasses);
			GBL::Frame_t quantized(frame.rows, frame.cols, frame.type());

			for(uint16_t row = 0; row < frame.rows; ++row) {
				const ValueType* srcRowPtr = frame.ptr(row);
				ValueType* dstRowPtr = quantized.ptr(row);

				for(uint16_t col = 0; col < frame.cols; ++col) {
					// Quantize value
					*dstRowPtr = quantizedBitExpansion(*srcRowPtr, shifter);

					// Shift to the next value
					++srcRowPtr;
					++dstRowPtr;
				}
			}
			return quantized;
		}

		template <typename ValueType, unsigned nbOfClasses>
		GBL::Frame_t MetaQuantization<ValueType, nbOfClasses>::quantizedBitExpansion(const GBL::Frame_t& frame) {
			STATIC_ASSERT(nbOfClasses > 0, "Nb of classes should be bigger than 0.");
			STATIC_ASSERT(nbOfClasses <= GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType), "Nb of classes should be equal to or smaller than the number of bits in the given ValueType");

			const ValueType shifter = GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType) - Utils::Meta::Log2<nbOfClasses>::result;
			GBL::Frame_t quantized(frame.rows, frame.cols, frame.type());

			for(uint16_t row = 0; row < frame.rows; ++row) {
				const ValueType* srcRowPtr = frame.ptr(row);
				ValueType* dstRowPtr = quantized.ptr(row);

				for(uint16_t col = 0; col < frame.cols; ++col) {
					// Quantize value
					*dstRowPtr = Quantization<ValueType>::quantizedBitExpansion(*srcRowPtr, shifter);

					// Shift to the next value
					++srcRowPtr;
					++dstRowPtr;
				}
			}
			return quantized;
		}

		template <typename ValueType>
		GBL::Frame_t MetaQuantization<ValueType, 1U>::quantizedBitExpansion(const GBL::Frame_t& frame) {
			// Return image with the same dimensions and type, containing all ones
			return GBL::Frame_t::ones(frame.rows, frame.cols, frame.type());
		}

		// Seems like we are counting on copy elision for this one
		template <typename ValueType, typename VectorType, unsigned nbOfClasses>
		GBL::Frame_t VectorQuantization<ValueType, VectorType, nbOfClasses>::quantizedBitExpansion(const GBL::Frame_t& frame) {
			// ----------------------------- Assert input ---------------------------//
			STATIC_ASSERT(nbOfClasses > 0, "nbOfClasses should be bigger than 0");
			STATIC_ASSERT(nbOfClasses <= GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType), "nbOfClasses should be smaller than the number of bits of in the ValueType");
			STATIC_ASSERT(sizeof(VectorType) % sizeof(ValueType) == 0, "Size of the VectorType should be an integer multiple of the ValueType");

			// ----------------------------- Compile time calculations ------------------//
			const ValueType shifter = GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType) - Utils::Meta::Log2<nbOfClasses>::result;
			// Make sure that the ValueType fits an integer number of times in the VectorType
			const ValueType valueTypesPerVector = sizeof(VectorType)/sizeof(ValueType);

			// ----------------------------- Run time ---------------------------//
			GBL::Frame_t quantized(frame.rows, frame.cols, frame.type());
			for(uint16_t row = 0; row < frame.rows; ++row) {
				// Reintepret data pointers to be pointing to VectorTypes instead of Valuetypes
				const VectorType* srcRowPtr = reinterpret_cast<const VectorType*>(frame.ptr(row));
				VectorType* dstRowPtr = reinterpret_cast<VectorType*>(quantized.ptr(row));

				for(uint16_t col = 0; col < frame.cols/valueTypesPerVector; ++col) {
					// Shift a bit to the appropriate place
					*dstRowPtr = quantizedBitExpansion(*srcRowPtr);

					// Shift to the next value
					++srcRowPtr;
					++dstRowPtr;
				}

				// Handle the leftovers at the side that did not fit into VectorType
				const ValueType* srcRowPtrSide = reinterpret_cast<const ValueType*>(srcRowPtr);
				ValueType* dstRowPtrSide = reinterpret_cast<ValueType*>(dstRowPtr);
				for(uint16_t col = 0; col < frame.cols % valueTypesPerVector; ++col) {
					// Quantize value
					*dstRowPtrSide = Quantization<ValueType>::quantizedBitExpansion(*srcRowPtrSide, shifter);

					// Increment pointers
					++srcRowPtrSide;
					++dstRowPtrSide;
				}
			}
			return quantized;
		}

		template <typename ValueType, typename VectorType, unsigned nbOfClasses>
		VectorType VectorQuantization<ValueType, VectorType, nbOfClasses>::quantizedBitExpansion(const VectorType value) {
			// ------------------- Assert input ----------------------------- //
			STATIC_ASSERT(nbOfClasses > 0, "nbOfClasses should be bigger than 0");
			STATIC_ASSERT(nbOfClasses <= GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType), "nbOfClasses should be smaller than the number of bits of in the ValueType");
			STATIC_ASSERT(sizeof(VectorType) % sizeof(ValueType) == 0, "Size of the VectorType should be an integer multiple of the ValueType");

			// ------------------- compile time calculations -------------------------//
			const ValueType shiftValue = sizeof(ValueType) * GBL::NB_BITS_IN_A_BYTE - Utils::Meta::Log2<nbOfClasses>::result;
			const ValueType valueTypesPerVector = sizeof(VectorType)/sizeof(ValueType);

			// ------------------- run time calculations ------------------------- //
			VectorConversion vectorConversion;

			// Calculate the value to shift for
			// This can be done VectorType
			vectorConversion.vector = EmbeddedMT::Vector::shiftRight<ValueType, VectorType, shiftValue>(value);

			// Shift 1 with the according shift value for each ValueType in the vector
			// Would be nice if we could vectorize this further, for now it is not
			for(uint8_t i = 0; i < valueTypesPerVector; ++i) {
				assert(vectorConversion.values[i] < GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType));
				vectorConversion.values[i] = 0x01 << vectorConversion.values[i];	
			}
			return vectorConversion.vector;
		}

		template <typename ValueType, typename VectorType>
		GBL::Frame_t VectorQuantization<ValueType, VectorType, 1U>::quantizedBitExpansion(const GBL::Frame_t& frame) {
			return GBL::Frame_t::ones(frame.rows, frame.cols, frame.type());
		}
	}
}

#endif
