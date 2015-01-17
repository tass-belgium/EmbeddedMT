#ifndef SHIFT_HPP
#define SHIFT_HPP

#include "cm/global.hpp"
#include "cm/meta.hpp"

namespace EmbeddedMT {
	namespace Vector {
		template <typename ValueType, typename VectorType, unsigned shiftValue>
		static inline VectorType shiftRight(const VectorType value) {
			// ----------------------- assert input ----------------------------//
			STATIC_ASSERT(shiftValue <= GBL::NB_BITS_IN_A_BYTE * sizeof(ValueType), "nbOfClasses should be smaller than the number of bits of in the ValueType");

			// ----------------------- compile time calculations --------------------------//
			const ValueType valueTypesPerVector = sizeof(VectorType)/sizeof(ValueType);
			const ValueType allOnesUntil = sizeof(ValueType) * GBL::NB_BITS_IN_A_BYTE - shiftValue;
			const ValueType mask = Utils::Meta::AllOnesUntil<allOnesUntil>::value;
			const VectorType shiftMask = Utils::Meta::RepeatTypedX<ValueType, VectorType, mask, valueTypesPerVector - 1>::value;

			// ----------------------- run time -------------------------//
			return (value >> shiftValue) & shiftMask;
		}	
	}
}

#endif
