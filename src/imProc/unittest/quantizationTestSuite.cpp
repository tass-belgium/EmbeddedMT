#include <stdlib.h>
#include <limits.h>
#include <iostream>
#include <string>

#include "cm/global.hpp"
#include "check.h"
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs/imgcodecs.hpp"

#include "outputMethod/outputImageSequence.hpp"

#include "quantizationTestSuite.hpp"
#include "../quantization.hpp"
#include "../slowQuantization.hpp"

using std::cout;
using std::endl;
using std::string;
using std::stringstream;
using EmbeddedMT::GBL::Frame_t;
using EmbeddedMT::ImageProc::Quantization;
using EmbeddedMT::ImageProc::SlowQuantization;
using EmbeddedMT::OutputMethod::OutputImageSequence;

namespace EmbeddedMT
{
	namespace test 
	{
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
		static void checkValidExpansion(const Frame_t& frame, const uint32_t line = __LINE__) {
			for (int32_t row = 0; row < frame.rows; ++row) {
				const valueType* framePtr = frame.ptr(row);
				for(int32_t col = 0; col < frame.cols; ++col) {
					ck_assert_msg(*framePtr != 0, "Line %d: Row %d col %d of expansion is 0", line, row, col);
					++framePtr;
				}
			}
		}

		template <typename valueType>
		static void compareFrames(const Frame_t& frame1, const Frame_t& frame2, const uint32_t line) {
			ck_assert_int_eq(frame1.rows, frame2.rows);
			ck_assert_int_eq(frame1.cols, frame2.cols);
			ck_assert_int_eq(frame1.dims, frame2.dims);
			ck_assert_int_eq(frame1.type(), frame2.type());

			for (int32_t row = 0; row < frame1.rows; ++row) {
				const valueType* frame1Ptr = frame1.ptr(row);
				const valueType* frame2Ptr = frame2.ptr(row);
				for(int32_t col = 0; col < frame1.cols; ++col) {
					// Workaround because there seems to be a bug in using ck_assert() on the frameptr values
					if(*frame1Ptr != *frame2Ptr) {
						cout << "CompareFrames call at line " << line << ", row " << row << " and col " << col << ", does not match. Value was " << int(*frame2Ptr) << " but should be " << int(*frame1Ptr) << endl;
						ck_assert(false);
					}	
					++frame1Ptr;
					++frame2Ptr;
				}
			}
		}

		START_TEST(testBitExpansionSimple)
		{
			const uint8_t nbOfClasses = 2U;
			const uint8_t testSize = 0x7F;
			Frame_t testImage = Frame_t(testSize, 1, CV_8UC1);				
			for (uint8_t i = 0; i < testSize; ++i) {
				testImage.at<uint8_t>(i, 0) = i;
			}
			Frame_t resultImage = Quantization<uint8_t>::quantizedBitExpansion(testImage, nbOfClasses);
			checkValidExpansion<uint8_t>(resultImage, __LINE__);
			Frame_t slowImage = SlowQuantization<uint8_t>::quantizedBitExpansion(testImage,nbOfClasses);
			checkValidExpansion<uint8_t>(slowImage, __LINE__);
			Frame_t correctResult = Frame_t::ones(testSize,1, CV_8UC1);
			compareFrames<uint8_t>(correctResult, resultImage, __LINE__);
			compareFrames<uint8_t>(correctResult, slowImage, __LINE__);
			
			for (uint8_t i = 0; i < testSize; ++i) {
				testImage.at<uint8_t>(i, 0) = 0x80 + i;
			}
			resultImage = Quantization<uint8_t>::quantizedBitExpansion(testImage, nbOfClasses);
			checkValidExpansion<uint8_t>(resultImage, __LINE__);
			slowImage = SlowQuantization<uint8_t>::quantizedBitExpansion(testImage, nbOfClasses);
			checkValidExpansion<uint8_t>(slowImage, __LINE__);
			correctResult = 2*Frame_t::ones(testSize,1, CV_8UC1);
			compareFrames<uint8_t>(correctResult, resultImage, __LINE__);
			compareFrames<uint8_t>(correctResult, slowImage, __LINE__);
		}
		END_TEST

		START_TEST(testBitExpansionSimple2)
		{
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

			Frame_t resultImage = Quantization<uint8_t>::quantizedBitExpansion(tmpTestImage, nbOfClasses);
			checkValidExpansion<uint8_t>(resultImage, __LINE__);
			Frame_t slowImage = SlowQuantization<uint8_t>::quantizedBitExpansion(tmpTestImage, nbOfClasses);
			checkValidExpansion<uint8_t>(slowImage, __LINE__);

			compareFrames<uint8_t>(tmpCorrectImage, slowImage, __LINE__);
			compareFrames<uint8_t>(tmpCorrectImage, resultImage, __LINE__);
		}
		END_TEST

		START_TEST(testBitExpansionRandom)
		{
			srand(0);
			uint8_t nbOfClasses = 0x01U;
			const uint32_t testSizeCol = 1280;
			const uint32_t testSizeRow = 600;

			while(nbOfClasses <= 8 && nbOfClasses > 0) {
				Frame_t testImage = Frame_t(testSizeRow, testSizeCol, CV_8UC1);
				for(uint32_t row = 0; row < testSizeRow; ++row) {
					for(uint32_t col = 0; col < testSizeCol; ++col) {
						testImage.at<uint8_t>(row, col) = rand() % std::numeric_limits<uint8_t>::max();
					}
				}
				
				Frame_t resultImage = Quantization<uint8_t>::quantizedBitExpansion(testImage, nbOfClasses);
				checkValidExpansion<uint8_t>(resultImage, __LINE__);
				Frame_t slowImage = SlowQuantization<uint8_t>::quantizedBitExpansion(testImage, nbOfClasses);
				checkValidExpansion<uint8_t>(slowImage, __LINE__);
			
				writeImage(testImage, "QuantizationTest-bitexpansion-random-original");	
				writeImage(resultImage, "QuantizationTest-bitexpansion-random-fastQuantization");
				writeImage(slowImage, "QuantizationTest-bitexpansion-random-slowQuantization");

				compareFrames<uint8_t>(slowImage, resultImage, __LINE__);
				
				// Repeat for 2^(nbOfClasses + 1)
				nbOfClasses <<= 1U;
			}

		}
		END_TEST

		START_TEST(testBitExpansionImage)
		{
			uint8_t nbOfClasses = 0x01U;
			while(nbOfClasses <= 8 && nbOfClasses > 0) {
				cout << "Executing test for " << int(nbOfClasses) << " quantization class(es)." << endl;
				Frame_t testImage = cv::imread("testSequences/test/duomo.jpg", cv::IMREAD_GRAYSCALE);
				
				Frame_t resultImage = Quantization<uint8_t>::quantizedBitExpansion(testImage, nbOfClasses);
				checkValidExpansion<uint8_t>(resultImage, __LINE__);
				Frame_t slowImage = SlowQuantization<uint8_t>::quantizedBitExpansion(testImage, nbOfClasses);
				checkValidExpansion<uint8_t>(slowImage, __LINE__);
				
				stringstream ss_test;
				ss_test << "QuantizationTest-bitexpansion-image-" << int(nbOfClasses) << "-original";
				writeImage(testImage, ss_test.str().c_str());
				stringstream ss_result;
				ss_result << "QuantizationTest-bitexpansion-image-" << int(nbOfClasses) << "-fastQuantization";
				writeImage(resultImage, ss_result.str().c_str());
				stringstream ss_slow;
				ss_slow << "QuantizationTest-bitexpansion-image-" << int(nbOfClasses) << "-slowQuantization";
				writeImage(slowImage, ss_slow.str().c_str());

				// Unpack bit expansion
				stringstream ss_unpacked_slow;
				ss_unpacked_slow << "QuantizationTest-bitexpansion-image-" << int(nbOfClasses) << "-unpacked-slowQuantization";
				writeImage(SlowQuantization<uint8_t>::unpackBitExpansion(slowImage, nbOfClasses), ss_unpacked_slow.str().c_str());
				stringstream ss_unpacked_fast;
				ss_unpacked_fast << "QuantizationTest-bitexpansion-image-" << int(nbOfClasses) << "-unpacked-fastQuantization";
				writeImage(SlowQuantization<uint8_t>::unpackBitExpansion(resultImage, nbOfClasses), ss_unpacked_fast.str().c_str());

 				compareFrames<uint8_t>(slowImage, resultImage, __LINE__);

				// Repeat for 2^(nbOfClasses + 1)
				nbOfClasses <<= 1U;
			}
		}
		END_TEST

		Suite * quantizationTestSuite(void)
		{
			Suite *s;
			TCase *tc_core;

			s = suite_create("Quantization tests");

			tc_core = tcase_create("Simple test");
			tcase_add_test(tc_core, testBitExpansionSimple);
			suite_add_tcase(s, tc_core);

			tc_core = tcase_create("Simple test 2");
			tcase_add_test(tc_core, testBitExpansionSimple2);
			suite_add_tcase(s, tc_core);
			
			tc_core = tcase_create("Random test");
			tcase_add_test(tc_core, testBitExpansionRandom);
			tcase_set_timeout(tc_core, 120);
			suite_add_tcase(s, tc_core);
			
			tc_core = tcase_create("Image test");
			tcase_add_test(tc_core, testBitExpansionImage);
			tcase_set_timeout(tc_core, 120);
			suite_add_tcase(s, tc_core);
			return s;
		}
	}
}
