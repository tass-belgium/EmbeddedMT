#include <random>
#include <limits>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "cm/global.hpp"
#include "testUtils/simdUtils.hpp"

#include "../shift.hpp"

using EmbeddedMT::Vector::shiftRight;

namespace {
	typedef uint8_t ValueType;
	typedef uint32_t VectorType;
	typedef EmbeddedMT::test::ValueVectorConversion<ValueType, VectorType> ValueVectorConversion_t;
}
namespace EmbeddedMT {
	namespace test {
		template <typename ValueType, typename VectorType, unsigned shiftFactor, unsigned... args>
		class GenerateShiftRight {
			public:
				static inline void generate(std::vector<ValueVectorConversion_t >& result, ValueVectorConversion_t& testData) {
					GenerateShiftRight<ValueType, VectorType, shiftFactor>::generate(result, testData);
					GenerateShiftRight<ValueType, VectorType, args...>::generate(result, testData);
				}
		};

		template <typename ValueType, typename VectorType, unsigned shiftFactor>
		class GenerateShiftRight<ValueType, VectorType, shiftFactor> {
			public:
				static inline void generate(std::vector<ValueVectorConversion_t >& result, ValueVectorConversion_t& testData) {
					const uint32_t vectorizationOrder = sizeof(VectorType)/sizeof(ValueType);
					ValueVectorConversion_t randomTestResultShiftFactor;
					for(auto it = testData.vectorTypeBegin();  it != testData.vectorTypeEnd(); ++it) {
						VectorType foundResult = shiftRight<ValueType, VectorType, shiftFactor>(*it);
						randomTestResultShiftFactor.pushBackVectorType(foundResult);
					}
					result.push_back(randomTestResultShiftFactor);		
				}
		};

		SCENARIO("Straightforward test for the vectorized shift right", "[Vector][ShiftRight]") {
			GIVEN("Some test value, shift values and their result") {
				const VectorType test = 0x1080F0FFU;	
				const VectorType resultOneBitShift = 0x0840787FU;

				srand(0);
				const uint32_t vectorizationOrder = sizeof(VectorType)/sizeof(ValueType);
				const uint32_t testLength = 1024U;
				REQUIRE((testLength % vectorizationOrder) == 0); // To make the test logic easy, force length to be a multiple of the vectorization order
				const uint32_t maxShiftValue = sizeof(ValueType) * GBL::NB_BITS_IN_A_BYTE;
				ValueVectorConversion_t randomTest;

				for(uint32_t i = 0; i < testLength; ++i) {
					randomTest.pushBackValueType(rand() % std::numeric_limits<ValueType>::max());
				}

				std::vector<ValueVectorConversion_t > randomTestCorrectResults;
				for(uint32_t shiftFactor = 0; shiftFactor < maxShiftValue; ++shiftFactor) {
					ValueVectorConversion_t randomTestOneBitShift;
					for(auto it = randomTest.valueTypeBegin(); it != randomTest.valueTypeEnd(); ++it) {
						randomTestOneBitShift.pushBackValueType(*it >> (shiftFactor));
					}
					randomTestCorrectResults.push_back(randomTestOneBitShift);
				}

				WHEN("We apply the same shifts to the vectorized shift right") {
					const VectorType result = shiftRight<ValueType, VectorType, 1>(test);

					std::vector<ValueVectorConversion_t > randomTestResult;
					GenerateShiftRight<ValueType, VectorType, 0, 1, 2, 3, 4, 5, 6, 7, 8>::generate(randomTestResult, randomTest);

					THEN("We should get the same result") {
						REQUIRE(result == resultOneBitShift);

						for(uint32_t shiftFactor = 0; shiftFactor < maxShiftValue; ++shiftFactor) {
							ValueVectorConversion_t& correctResult = randomTestCorrectResults[shiftFactor];
							ValueVectorConversion_t foundResult = randomTestResult[shiftFactor];
							for(uint32_t i = 0; i < testLength; ++i) {
								REQUIRE(foundResult.atValueType(i) == correctResult.atValueType(i));
							}
						}
					}
				}
			}
		}
	}
}
