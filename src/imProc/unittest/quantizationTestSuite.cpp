#include <stdlib.h>
#include <limits.h>
#include <iostream>
#include <string>
#include <map>

#include "catch.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"

#include "cm/global.hpp"
#include "outputMethod/outputImageSequence.hpp"
#include "testUtils/testUtils.hpp"

#include "../quantization.hpp"
#include "../slowQuantization.hpp"


using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using std::map;
using std::vector;

using EmbeddedMT::GBL::Frame_t;
using EmbeddedMT::ImageProc::Quantization;
using EmbeddedMT::ImageProc::SlowQuantization;
using EmbeddedMT::ImageProc::MetaQuantization;
using EmbeddedMT::ImageProc::VectorQuantization;
using EmbeddedMT::OutputMethod::OutputImageSequence;

using EmbeddedMT::test::Utils;

namespace EmbeddedMT
{
	namespace test 
	{
		template<typename Type, unsigned T, unsigned... args>
		class GenerateTemplate {
			public:
				static inline map<Type, Frame_t> generate(const Frame_t& image) {
					map<Type, Frame_t> map = GenerateTemplate<Type, args...>::generate(image);

					Frame_t quantized = VectorQuantization<uint8_t, uint32_t, T>::quantizedBitExpansion(image);
					map.insert(std::pair<Type, Frame_t>(T, quantized));
					return map;
				}
		};
		
		template<typename Type, unsigned T>
		class GenerateTemplate<Type, T> {
			public:
				static inline map<Type, Frame_t> generate(const Frame_t& image) {
					map<Type, Frame_t> map;

					Frame_t quantized = VectorQuantization<uint8_t, uint32_t, T>::quantizedBitExpansion(image);
					map.insert(std::pair<Type, Frame_t>(T, quantized));
					return map;
				}
		};

		void writeImage(const Frame_t& frame, const char* filename) {
			OutputImageSequence output;
			output.open(filename); 			
			output.write(frame);
			output.close();
		}

		void writeImage(const Frame_t& frame, string filename) {
			writeImage(frame, filename.c_str());
		}

		template <typename valueType>
		static void checkValidExpansion(const Frame_t& frame) {
			for (int32_t row = 0; row < frame.rows; ++row) {
				const valueType* framePtr = frame.ptr(row);
				for(int32_t col = 0; col < frame.cols; ++col) {
					REQUIRE(*framePtr != 0);
					++framePtr;
				}
			}
		}
		

 		SCENARIO("Test Quantization with bit expansion for value 0x0 - 0x7F for 8 bit values, two quantization classes", "[Quantization][BitExpansion]")
		{
			GIVEN("A number of classes, a test image and its result") {
				const uint8_t nbOfClasses = 2U;
				const uint8_t testSizeX = 0x7F;
				const uint8_t testSizeY = 5U;
				Frame_t testImage = Frame_t(testSizeX, testSizeY, CV_8UC1);				
				for (uint8_t i = 0; i < testSizeX; ++i) {
					for(uint8_t j = 0; j < testSizeY; ++j) {
						testImage.at<uint8_t>(i, j) = i;
					}	
				}
				Frame_t correctResult = Frame_t::ones(testSizeX, testSizeY, CV_8UC1);
				
				WHEN("We quantize using the Slow Quantization method") {
					Frame_t image = SlowQuantization<uint8_t>::quantizedBitExpansion(testImage,nbOfClasses);
					
					THEN("The image should be a valid bit expansion") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The image should match the result") {
						Utils::compareFrames<uint8_t>(correctResult, image);
					}
				}

				WHEN("We quantize using the Fast Quantization Method") {
					Frame_t image = Quantization<uint8_t>::quantizedBitExpansion(testImage, nbOfClasses);
						
					THEN("The image should be a valid bit expansion") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The image should match the result") {
						Utils::compareFrames<uint8_t>(correctResult, image);
					}
				}

				WHEN("We quantize using the Meta Quantization Method") {
					Frame_t image = MetaQuantization<uint8_t, nbOfClasses>::quantizedBitExpansion(testImage);
						
					THEN("The image should be a valid bit expansion") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The image should match the result") {
						Utils::compareFrames<uint8_t>(correctResult, image);
					}
				}

				WHEN("We quantize using the Vector Quantization Method with the same type") {
					Frame_t image = VectorQuantization<uint8_t, uint8_t, nbOfClasses>::quantizedBitExpansion(testImage);
						
					THEN("The image should be a valid bit expansion") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The image should match the result") {
						Utils::compareFrames<uint8_t>(correctResult, image);
					}
				}

				WHEN("We quantize using the Vector Quantization Method with the double vector type") {
					Frame_t image = VectorQuantization<uint8_t, uint16_t, nbOfClasses>::quantizedBitExpansion(testImage);
						
					THEN("The image should be a valid bit expansion") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The image should match the result") {
						Utils::compareFrames<uint8_t>(correctResult, image);
					}
				}

				WHEN("We quantize using the Vector Quantization Method with four times vector type") {
					Frame_t image = VectorQuantization<uint8_t, uint32_t, nbOfClasses>::quantizedBitExpansion(testImage);
						
					THEN("The image should be a valid bit expansion") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The image should match the result") {
						Utils::compareFrames<uint8_t>(correctResult, image);
					}
				}

				WHEN("We quantize using the Vector Quantization Method with eight times vector type") {
					Frame_t image = VectorQuantization<uint8_t, uint64_t, nbOfClasses>::quantizedBitExpansion(testImage);
						
					THEN("The image should be a valid bit expansion") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The image should match the result") {
						Utils::compareFrames<uint8_t>(correctResult, image);
					}
				}
			}
		}

		SCENARIO("Test Quantization with bit expansion for value 0x80 - 0xFF for 8 bit values, two quantization classes", "[Quantization][BitExpansion]")
		{
			GIVEN("A number of classes, a test image and its result") {
				const uint8_t nbOfClasses = 2U;
				const uint8_t testSizeX = 0x7F;
				const uint8_t testSizeY = 5U;
				Frame_t testImage = Frame_t(testSizeX, testSizeY, CV_8UC1);				
				for (uint8_t i = 0; i < testSizeX; ++i) {
					for(uint8_t j = 0; j < testSizeY; ++j) {
						testImage.at<uint8_t>(i, j) = 0x80 + i;
					}	
				}
				Frame_t correctResult = 2*Frame_t::ones(testSizeX, testSizeY, CV_8UC1);
				
				WHEN("We quantize using the Slow Quantization method") {
					Frame_t image = SlowQuantization<uint8_t>::quantizedBitExpansion(testImage,nbOfClasses);
					
					THEN("The image should be a valid bit expansion") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The image should match the result") {
						Utils::compareFrames<uint8_t>(correctResult, image);
					}
				}

				WHEN("We quantize using the Fast Quantization Method") {
					Frame_t image = Quantization<uint8_t>::quantizedBitExpansion(testImage, nbOfClasses);
						
					THEN("The image should be a valid bit expansion") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The image should match the result") {
						Utils::compareFrames<uint8_t>(correctResult, image);
					}
				}

				WHEN("We quantize using the Meta Quantization Method") {
					Frame_t image = MetaQuantization<uint8_t, nbOfClasses>::quantizedBitExpansion(testImage);
						
					THEN("The image should be a valid bit expansion") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The image should match the result") {
						Utils::compareFrames<uint8_t>(correctResult, image);
					}
				}

				WHEN("We quantize using the Vector Quantization Method with the same type") {
					Frame_t image = VectorQuantization<uint8_t, uint8_t, nbOfClasses>::quantizedBitExpansion(testImage);
						
					THEN("The image should be a valid bit expansion") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The image should match the result") {
						Utils::compareFrames<uint8_t>(correctResult, image);
					}
				}

				WHEN("We quantize using the Vector Quantization Method with the double vector type") {
					Frame_t image = VectorQuantization<uint8_t, uint16_t, nbOfClasses>::quantizedBitExpansion(testImage);
						
					THEN("The image should be a valid bit expansion") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The image should match the result") {
						Utils::compareFrames<uint8_t>(correctResult, image);
					}
				}

				WHEN("We quantize using the Vector Quantization Method with four times vector type") {
					Frame_t image = VectorQuantization<uint8_t, uint32_t, nbOfClasses>::quantizedBitExpansion(testImage);
						
					THEN("The image should be a valid bit expansion") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The image should match the result") {
						Utils::compareFrames<uint8_t>(correctResult, image);
					}
				}

				WHEN("We quantize using the Vector Quantization Method with eight times vector type") {
					Frame_t image = VectorQuantization<uint8_t, uint64_t, nbOfClasses>::quantizedBitExpansion(testImage);
						
					THEN("The image should be a valid bit expansion") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The image should match the result") {
						Utils::compareFrames<uint8_t>(correctResult, image);
					}
				}
			}
		}

		SCENARIO("Test Quantization with bit expansion for hand made test and solution frame", "[Quantization][BitExpansion]")
		{
			GIVEN("A nb of classes, a test image and the correct result") {
				const uint8_t nbOfClasses = 4U;
				const uint32_t imageLength = 10;
				uint8_t testImage[imageLength*imageLength] = {    		  	  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
																			  0, 128, 140, 151, 162, 173, 184, 155, 191,   0,
																			  0,  64, 255, 255, 255, 255, 255, 255, 192,   0,
																			  0,  67, 255, 255, 255, 255, 255, 255, 201,   0,
																			  0, 105, 255, 255, 255, 255, 255, 255, 213,   0,
																			  0,  77, 255, 255, 255, 255, 255, 255, 224,   0,
																			  0, 110, 255, 255, 255, 255, 255, 255, 239,   0,
																			  0, 127, 255, 255, 255, 255, 255, 255, 255,   0,
																			  0,   1,  12,  23,  34,  45,  56,  60,  63,   0,
																			  0, 0,   0,   0,   0,   0,   0,   0,   0,   0};

				Frame_t tmpTestImage(imageLength, imageLength, CV_8UC1, testImage);
				uint8_t correctImage[imageLength*imageLength] = {    		0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U,
																			0x01U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x04U, 0x01U,
																			0x01U, 0x02U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x01U,
																			0x01U, 0x02U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x01U,
																			0x01U, 0x02U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x01U,
																			0x01U, 0x02U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x01U,
																			0x01U, 0x02U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x01U,
																			0x01U, 0x02U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x08U, 0x01U,
																			0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U,
																			0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U, 0x01U};
				Frame_t tmpCorrectImage(imageLength, imageLength, CV_8UC1, correctImage);

				WHEN("We apply slow quantization on the frame") {
					Frame_t image = SlowQuantization<uint8_t>::quantizedBitExpansion(tmpTestImage, nbOfClasses);
					THEN("The resulting image should be valid") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The resulting image should be the same as the correct result") {
						Utils::compareFrames<uint8_t>(tmpCorrectImage, image);
					}
				}
				
				WHEN("We apply fast quantization on the frame") {
					Frame_t image = Quantization<uint8_t>::quantizedBitExpansion(tmpTestImage, nbOfClasses);
					THEN("The resulting image should be valid") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The resulting image should be the same as the correct result") {
						Utils::compareFrames<uint8_t>(tmpCorrectImage, image);
					}
				}

				WHEN("We apply meta quantization on the frame") {
					Frame_t image = MetaQuantization<uint8_t, nbOfClasses>::quantizedBitExpansion(tmpTestImage);
					THEN("The resulting image should be valid") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The resulting image should be the same as the correct result") {
						Utils::compareFrames<uint8_t>(tmpCorrectImage, image);
					}
				}

				WHEN("We apply vector quantization on the frame with the same type") {
					Frame_t image = VectorQuantization<uint8_t, uint8_t, nbOfClasses>::quantizedBitExpansion(tmpTestImage);
					THEN("The resulting image should be valid") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The resulting image should be the same as the correct result") {
						Utils::compareFrames<uint8_t>(tmpCorrectImage, image);
					}
				}

				WHEN("We apply vector quantization on the frame with a vector type twice the value type") {
					Frame_t image = VectorQuantization<uint8_t, uint16_t, nbOfClasses>::quantizedBitExpansion(tmpTestImage);
					THEN("The resulting image should be valid") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The resulting image should be the same as the correct result") {
						Utils::compareFrames<uint8_t>(tmpCorrectImage, image);
					}
				}

				WHEN("We apply vector quantization on the frame with a vector type four times the value type") {
					Frame_t image = VectorQuantization<uint8_t, uint32_t, nbOfClasses>::quantizedBitExpansion(tmpTestImage);
					THEN("The resulting image should be valid") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The resulting image should be the same as the correct result") {
						Utils::compareFrames<uint8_t>(tmpCorrectImage, image);
					}
				}

 				WHEN("We apply vector quantization on the frame with a vector type eight times the value type") {
					Frame_t image = VectorQuantization<uint8_t, uint64_t, nbOfClasses>::quantizedBitExpansion(tmpTestImage);
					THEN("The resulting image should be valid") {
						checkValidExpansion<uint8_t>(image);
					}
					THEN("The resulting image should be the same as the correct result") {
						Utils::compareFrames<uint8_t>(tmpCorrectImage, image);
					}
				}
			}
		}

		SCENARIO("Test Quantization with bit expansion for frame with random values", "[Quantization][BitExpansion]")
		{
			GIVEN("An image filled with random values and the result image") {
				srand(0);
				vector<uint8_t> nbOfClassesList = { 1U, 2U, 4U, 8U };
				const uint32_t testSizeCol = 1280;
				const uint32_t testSizeRow = 600;

				// Construct image with random values
				Frame_t testImage = Frame_t(testSizeRow, testSizeCol, CV_8UC1);
				for(uint32_t row = 0; row < testSizeRow; ++row) {
					for(uint32_t col = 0; col < testSizeCol; ++col) {
						testImage.at<uint8_t>(row, col) = rand() % std::numeric_limits<uint8_t>::max();
					}
				}
				writeImage(testImage, "QuantizationTest-bitexpansion-random-original");	

				map<uint8_t, Frame_t> correctImages;
				for(auto nbOfClasses : nbOfClassesList) {
					Frame_t correctImage = SlowQuantization<uint8_t>::quantizedBitExpansion(testImage, nbOfClasses);
					correctImages.insert(std::pair<uint8_t, Frame_t>(nbOfClasses, correctImage));
				}

				WHEN("We use the fast quantization method") {
					for(auto nbOfClasses : nbOfClassesList) {
						Frame_t image = Quantization<uint8_t>::quantizedBitExpansion(testImage, nbOfClasses);
						writeImage(image, "QuantizationTest-bitexpansion-random-fastQuantization");

						THEN("We should get a valid bit expansion image") {
							checkValidExpansion<uint8_t>(image);
						}
						THEN("We should get the same result") {
							Utils::compareFrames<uint8_t>(correctImages[nbOfClasses], image);
						}
					}
				}

				WHEN("We use the vector quantization method") {
					// For templates all values in the for-loop need to be pregenerated and precomputed
					map<uint8_t, Frame_t> map32QuantizationResults = GenerateTemplate<uint8_t, 1U, 2U, 4U, 8U>::generate(testImage);

					for(auto nbOfClasses : nbOfClassesList) {
						stringstream ss_result;
						ss_result << "QuantizationTest-bitexpansion-image-" << int(nbOfClasses) << "-vectorQuantization";
						writeImage(map32QuantizationResults[nbOfClasses], ss_result.str().c_str());

						THEN("We should get a valid bit expansion image") {
							checkValidExpansion<uint8_t>(map32QuantizationResults[nbOfClasses]);
						}
						THEN("We should get the same result") {
							Utils::compareFrames<uint8_t>(correctImages[nbOfClasses], map32QuantizationResults[nbOfClasses]);
						}
					}
				}
			}
		}

		SCENARIO("Test Quantization with bit expansion with a real image", "[Quantization][BitExpansion]")
		{
			GIVEN("An image and its correct result") {
				srand(0);
				vector<uint8_t> nbOfClassesList = { 1U, 2U, 4U, 8U };
				Frame_t testImage = cv::imread("testSequences/test/duomo.jpg", cv::IMREAD_GRAYSCALE);

				stringstream ss_test;
				ss_test << "QuantizationTest-bitexpansion-image-original";
				writeImage(testImage, ss_test.str().c_str());

				map<uint8_t, Frame_t> correctImages;
				for(auto nbOfClasses : nbOfClassesList) {
					Frame_t correctImage = SlowQuantization<uint8_t>::quantizedBitExpansion(testImage, nbOfClasses);
					correctImages.insert(std::pair<uint8_t, Frame_t>(nbOfClasses, correctImage));

					stringstream ss_slow;
					ss_slow << "QuantizationTest-bitexpansion-image-" << int(nbOfClasses) << "-slowQuantization";
					writeImage(correctImage, ss_slow.str().c_str());
				}

				WHEN("We use the fast quantization method") {
					for(auto nbOfClasses : nbOfClassesList) {
						Frame_t image = Quantization<uint8_t>::quantizedBitExpansion(testImage, nbOfClasses);
						stringstream ss_result;
						ss_result << "QuantizationTest-bitexpansion-image-" << int(nbOfClasses) << "-fastQuantization";
						writeImage(image, ss_result.str().c_str());

						THEN("We should get a valid bit expansion image") {
							checkValidExpansion<uint8_t>(image);
						}
						THEN("We should get the same result") {
							Utils::compareFrames<uint8_t>(correctImages[nbOfClasses], image);
						}
					}
				}

				WHEN("We use the vector quantization method") {
					// For templates all values in the for-loop need to be pregenerated and precomputed
					map<uint8_t, Frame_t> map32QuantizationResults = GenerateTemplate<uint8_t, 1U, 2U, 4U, 8U>::generate(testImage);

					for(auto nbOfClasses : nbOfClassesList) {
						stringstream ss_result;
						ss_result << "QuantizationTest-bitexpansion-image-" << int(nbOfClasses) << "-vectorQuantization";
						writeImage(map32QuantizationResults[nbOfClasses], ss_result.str().c_str());

						THEN("We should get a valid bit expansion image") {
							checkValidExpansion<uint8_t>(map32QuantizationResults[nbOfClasses]);
						}
						THEN("We should get the same result") {
							Utils::compareFrames<uint8_t>(correctImages[nbOfClasses], map32QuantizationResults[nbOfClasses]);
						}
					}
				}
			}
		}
	}
}
