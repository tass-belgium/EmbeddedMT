#ifndef META_HPP
#define META_HPP

#include <iostream>

namespace EmbeddedMT {
	namespace Utils {
		namespace Meta {
			// Log2<value>::value will calculate floor(log2(int)) at compile time
			template <unsigned value>
			struct Log2 {
				// Compiler gets confused with the shift operator for the division by two. Since this is compile time, we can just divide by two
				enum { result = (1 + Log2<value/2>::result) };
			};

			// Specialization/stop criterium for value = 0
			template <>
			struct Log2<0> {
				enum { result = -1 };
			};

			
			// Repeat the given originalValue of the originalType nbOfTimesToRepeat times
			// Repeated value contained in RepeatX<Type, Value, Unsigned>::value at compile time
			// Note: the maximum value that can be held by the value member is limited by the enum data type. If this may not be sufficient, consider using the RepeatTypedX<> template structures.
			template <typename OriginalType, int originalValue, int nbOfTimesToRepeat>
			struct RepeatX {
				enum { value = (originalValue + (RepeatX<OriginalType, originalValue, nbOfTimesToRepeat - 1>::value << (GBL::NB_BITS_IN_A_BYTE * sizeof(OriginalType)))) };
			};

			template<typename OriginalType, int originalValue>
			struct RepeatX<OriginalType, originalValue, 0> {
				enum { value = originalValue };
			};

			// Data type of enum values is limited. Therefore this is a variant where the resultType is defined by an extra template argument
			template <typename OriginalType, typename ResultType, int originalValue, int nbOfTimesToRepeat>
			struct RepeatTypedX {
				static const ResultType value = (originalValue + (RepeatTypedX<OriginalType, ResultType, originalValue, nbOfTimesToRepeat - 1>::value << (GBL::NB_BITS_IN_A_BYTE * sizeof(OriginalType))));
			};

			template <typename OriginalType, typename ResultType, int originalValue>
			struct RepeatTypedX<OriginalType, ResultType, originalValue, 0> {
				static const ResultType value = originalValue;
			};

			// AllOnesUntil::value contains a value of which all bits are set to one up to the given position and zero for all other bits.
			// Calculation done at compile time
			template <unsigned position>
			struct AllOnesUntil {
				enum { value = ((AllOnesUntil<position-1>::value << 1) + 0x01) };
			};

			template <>
			struct AllOnesUntil<0> {
				enum { value = 0 };
			};

			template <typename ValueType, unsigned valueSize, unsigned size, bool ok>
			struct FitsInXHelper {
				enum { value = FitsInXHelper<ValueType, valueSize + sizeof(ValueType), size, valueSize >= size>::value };
			};

			template <typename ValueType, unsigned valueSize, unsigned size>
			struct FitsInXHelper<ValueType, valueSize, size, true> {
				enum { value = 1 };
			};

			template <typename ValueType, unsigned size>
			struct FitsInX {
				enum { value = FitsInXHelper<ValueType, sizeof(ValueType), size, sizeof(ValueType) >= size>::value };
			};
		}
	}
}
#endif
