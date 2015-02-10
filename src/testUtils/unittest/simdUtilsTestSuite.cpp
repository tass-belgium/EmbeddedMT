/*
 * =====================================================================================
 *
 *       Filename:  SimdUtilsTestSuite.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  01/22/2015 07:55:43 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Bart Verhagen (bv), bart.verhagen@tass.be
 *   Organization:  TASS
 *
 * =====================================================================================
 */
#include <limits>

#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#include "../simdUtils.hpp"

namespace EmbeddedMT {
	namespace test {
		typedef uint8_t ValueType;
		typedef uint32_t VectorType;


		SCENARIO("Straightforward test for simd Utils with array length a multiple of the value type per vector types", "[testUtils][simdUtils]") {
			GIVEN("A random array of valueTypes") {
				const std::size_t arrayLength = 1024U;
				ValueType testData[arrayLength];
				const uint8_t valueTypesPerVector = sizeof(VectorType)/sizeof(ValueType);

				// For this test, array length should be perfectly aligned for both the Value and VectorType
				REQUIRE((arrayLength % valueTypesPerVector) == 0);
				const std::size_t vectorArrayLength = arrayLength/valueTypesPerVector; // Since arraylength is a multiple of valueTypesPerVector, we can do this

				srand(0);
				
				for(uint32_t i = 0; i < arrayLength; ++i) {
					testData[i] = rand() % std::numeric_limits<ValueType>::max();
				}
				VectorType* vectorTestData = reinterpret_cast<VectorType*>(testData);

				WHEN("We add the items to the simd utils conversion class") {
					EmbeddedMT::test::ValueVectorConversion<ValueType, VectorType> simdUtils;
					for(uint32_t i = 0; i < arrayLength; ++i) {
						simdUtils.pushBackValueType(testData[i]);
					}

					THEN("The size of the ValueType array should be the same as the data length") {
						REQUIRE(simdUtils.sizeValueType() == arrayLength);
					}

					THEN("The size of the VectorType array should be the same as the data length divided by the number of ValueTypes that fit in a VectorType") {
						REQUIRE(simdUtils.sizeVectorType() == arrayLength / valueTypesPerVector);
					}

					THEN("We should get the same values when we access the data as a ValueType using its index") {
						for(uint32_t i = 0; i < arrayLength; ++i) {
							REQUIRE(testData[i] == simdUtils.atValueType(i));
						}	
					}

					THEN("We should get the same values when we access the data as a ValueType using the iterator") {
						uint32_t i = 0;
						for(auto it = simdUtils.valueTypeBegin(); it != simdUtils.valueTypeEnd(); ++it) {
							REQUIRE(*it == testData[i]);
							++i;
						}	
						REQUIRE(i == arrayLength);
					}

					THEN("We should get the same values when we access the data as a VectorType using its index") {
						for(uint32_t i = 0; i < vectorArrayLength; ++i) {
							REQUIRE(vectorTestData[i] == simdUtils.atVectorType(i));				
						}
					}

					THEN("We should get the same values when we access the data as a VectorType using the iterator") {
						uint32_t i = 0;
						for(auto it = simdUtils.vectorTypeBegin(); it != simdUtils.vectorTypeEnd(); ++it) {
							REQUIRE(*it == vectorTestData[i]);
							++i;
						}
						REQUIRE(i == vectorArrayLength);
					}
				}
			}
		}

		SCENARIO("Straightforward test for simd Utils with array length not a multiple of the value type per vector types", "[testUtils][simdUtils]") {
			GIVEN("A random array of valueTypes") {
				const std::size_t paddedArrayLength = 1028U;
				const std::size_t arrayLength = paddedArrayLength - 2U;
				ValueType testData[arrayLength + 2U]; // Pad with zeros, makes it easier to test
				const uint8_t valueTypesPerVector = sizeof(VectorType)/sizeof(ValueType);

				// For this test, array length should be perfectly aligned for both the Value and VectorType
				REQUIRE((arrayLength % valueTypesPerVector) != 0);
				const std::size_t fullVectorArrayLength = arrayLength/valueTypesPerVector;
				const std::size_t vectorArrayLength = fullVectorArrayLength + 1U; // The case if arrayLength is not a multiple of valueTypesPerVector

				srand(0);
				
				for(uint32_t i = 0; i < arrayLength; ++i) {
					testData[i] = rand() % std::numeric_limits<ValueType>::max();
				}
				for(uint32_t i = arrayLength; i < paddedArrayLength; ++i) {
					testData[i] = 0;
				}	
				VectorType* vectorTestData = reinterpret_cast<VectorType*>(testData);

				WHEN("We add the items to the simd utils conversion class") {
					EmbeddedMT::test::ValueVectorConversion<ValueType, VectorType> simdUtils;
					for(uint32_t i = 0; i < arrayLength; ++i) {
						simdUtils.pushBackValueType(testData[i]);
					}

					THEN("The size of the ValueType array should be the same as the data length") {
						REQUIRE(simdUtils.sizeValueType() == arrayLength);
					}

					THEN("The size of the VectorType array should be the same as the data length divided by the number of ValueTypes that fit in a VectorType plus one") {
						REQUIRE(simdUtils.sizeVectorType() == vectorArrayLength);
					}

					THEN("We should get the same values when we access the data as a ValueType using its index") {
						for(uint32_t i = 0; i < arrayLength; ++i) {
							REQUIRE(testData[i] == simdUtils.atValueType(i));
						}	
					}

					THEN("We should get the same values when we access the data as a ValueType using the iterator") {
						uint32_t i = 0;
						for(auto it = simdUtils.valueTypeBegin(); it != simdUtils.valueTypeEnd(); ++it) {
							REQUIRE(*it == testData[i]);
							++i;
						}	
						REQUIRE(i == arrayLength);
					}

					THEN("We should get the same values when we access the data as a VectorType using its index") {
						// Since we padded the test data with zeros, this test becomes a lot easier
						for(uint32_t i = 0; i < vectorArrayLength; ++i) {
							REQUIRE(vectorTestData[i] == simdUtils.atVectorType(i));				
						}
					}

					THEN("We should get the same values when we access the data as a VectorType using the iterator") {
						// Since we padded the test data with zeros, this test becomes a lot easier
						uint32_t i = 0;
						for(auto it = simdUtils.vectorTypeBegin(); it != simdUtils.vectorTypeEnd(); ++it) {
							REQUIRE(*it == vectorTestData[i]);
							++i;
						}
						REQUIRE(i == vectorArrayLength);
					}
				}
			}
		}

		SCENARIO("Push back the same values in as a value type and a vector type", "[testUtils][simdUtils]") {
			GIVEN("A test array") {
				// Must align: class cannot know whether padding is part of the collection or not, for vector input we assume they are
				const std::size_t arrayLength = 1024U;
				ValueType testData[arrayLength]; // Pad with zeros, makes it easier to test
				const uint8_t valueTypesPerVector = sizeof(VectorType)/sizeof(ValueType);

				// For this test, array length should be perfectly aligned for both the Value and VectorType
				REQUIRE((arrayLength % valueTypesPerVector) == 0);
				const std::size_t vectorArrayLength = arrayLength / valueTypesPerVector; // The case if arrayLength is a multiple of valueTypesPerVector

				srand(0);
				for(uint32_t i = 0; i < arrayLength; ++i) {
					testData[i] = rand() % std::numeric_limits<ValueType>::max();
				}
				VectorType* vectorTestData = reinterpret_cast<VectorType*>(testData);

				WHEN("We add the test array to the ValueVectorConversion class as vector types") {
					ValueVectorConversion<ValueType, VectorType> vectorInputTestObject;
					for(std::size_t i = 0; i < vectorArrayLength; ++i) {
						vectorInputTestObject.pushBackVectorType(vectorTestData[i]);	
					}
					THEN("We should get the same sizes as the test data") {
						REQUIRE(arrayLength == vectorInputTestObject.sizeValueType());
						REQUIRE(vectorArrayLength == vectorInputTestObject.sizeVectorType());
					}

					THEN("We should get the same result back out both as value and as vector types") {
						// Vector type iteration
						std::size_t i = 0;
						for(auto it = vectorInputTestObject.vectorTypeBegin(); it != vectorInputTestObject.vectorTypeEnd(); ++it) {
							REQUIRE(vectorTestData[i] == *it);
							++i;
						}
						REQUIRE(i == vectorArrayLength);
						// Value type iteration
						std::size_t j = 0;
						for(auto it = vectorInputTestObject.valueTypeBegin(); it != vectorInputTestObject.valueTypeEnd(); ++it) {
							REQUIRE(testData[j] == *it);
							++j;
						}
						REQUIRE(j == arrayLength);
					}
				}
			}
		}
	}
}
