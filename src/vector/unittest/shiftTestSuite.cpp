#include <random>
#include <limits>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "cm/global.hpp"

#include "../shift.hpp"

using EmbeddedMT::Vector::shiftRight;

typedef uint8_t ValueType;
typedef uint32_t VectorType;

namespace EmbeddedMT {
	namespace test {
		template <typename ValueType, typename VectorType, unsigned shiftFactor, unsigned... args>
		class GenerateShiftRight {
			public:
				static inline void generate(std::vector<std::vector<ValueType> >& result, const VectorType* vectorRandomTest, const uint32_t testLength) {
					GenerateShiftRight<ValueType, VectorType, shiftFactor>::generate(result, vectorRandomTest, testLength);
					GenerateShiftRight<ValueType, VectorType, args...>::generate(result, vectorRandomTest, testLength);
				}
		};

		template <typename ValueType, typename VectorType, unsigned shiftFactor>
		class GenerateShiftRight<ValueType, VectorType, shiftFactor> {
			public:
				static inline void generate(std::vector<std::vector<ValueType> >& result, const VectorType* vectorRandomTest, const uint32_t testLength) {
					const uint32_t vectorizationOrder = sizeof(VectorType)/sizeof(ValueType);
					std::vector<ValueType> randomTestResultShiftFactor;
					for(uint32_t i = 0;  i < testLength / vectorizationOrder; ++i) {
						VectorType foundResult = shiftRight<ValueType, VectorType, shiftFactor>(vectorRandomTest[i]);
						for(uint32_t valueIndex = 0; valueIndex < vectorizationOrder; ++valueIndex) {
							ValueType value = foundResult & std::numeric_limits<ValueType>::max();  
							randomTestResultShiftFactor.push_back(value);
							foundResult = foundResult >> sizeof(ValueType) * GBL::NB_BITS_IN_A_BYTE;
						}
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
				ValueType randomTest[testLength];

				for(uint32_t i = 0; i < testLength; ++i) {
					randomTest[i] = rand() % std::numeric_limits<ValueType>::max();
				}

				std::vector<std::vector<ValueType>> randomTestCorrectResults;
				for(uint32_t shiftFactor = 0; shiftFactor < maxShiftValue; ++shiftFactor) {
					std::vector<ValueType> randomTestOneBitShift;
					for(uint32_t i = 0; i < testLength; ++i) {
						randomTestOneBitShift.push_back(randomTest[i] >> (shiftFactor));
					}
					randomTestCorrectResults.push_back(randomTestOneBitShift);
				}
				const VectorType* vectorRandomTest = reinterpret_cast<VectorType*>(randomTest);

				WHEN("We apply the same shifts to the vectorized shift right") {
					const VectorType result = shiftRight<ValueType, VectorType, 1>(test);

					std::vector<std::vector<ValueType>> randomTestResult;
					GenerateShiftRight<ValueType, VectorType, 0, 1, 2, 3, 4, 5, 6, 7, 8>::generate(randomTestResult, vectorRandomTest, testLength);

					THEN("We should get the same result") {
						REQUIRE(result == resultOneBitShift);

						for(uint32_t shiftFactor = 0; shiftFactor < maxShiftValue; ++shiftFactor) {
							std::vector<ValueType> correctResult = randomTestCorrectResults[shiftFactor];
							std::vector<ValueType> foundResult = randomTestResult[shiftFactor];
							for(uint32_t i = 0; i < testLength; ++i) {
								REQUIRE(foundResult[i] == correctResult[i]);
							}
						}
					}
				}
			}
		}
	}
}
