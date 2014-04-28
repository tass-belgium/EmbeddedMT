/*
 * =====================================================================================
 *
 *       Filename:  testImProcBase.cpp
 *
 *    Description:  Test program for imProcBase
 *
 *        Version:  1.0
 *        Created:  04/22/2014 02:20:23 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Bart Verhagen (bv), bart.verhagen@tass.be
 *   Organization:  TASS
 *
 * =====================================================================================
 */


#include	<stdlib.h>
#include <iostream>
#include "cm/global.hpp"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"

#include "../imageProcBase.hpp"

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int main ( int argc, char *argv[] )
{
	ImageProc::ImageProcBase imProc;
	
	GBL::Image_t testImage(8,8,CV_8UC1);
	for(uint16_t i = 0; i < testImage.rows*testImage.cols; i++) {
		testImage.data[i] = 0;
	}
	testImage.data[27] = 0xff;
	GBL::Image_t mask(testImage.rows, testImage.cols, CV_8UC1);
	imProc.getMask(testImage, mask, 1);
	
	for(uint16_t i = 0; i < testImage.rows; i++) {
		for(uint16_t j = 0; j < testImage.cols; j++) {
			std::cout << (uint32_t) mask.data[i * testImage.cols + j] << ' ';
		}
		std::cout << std::endl;
	}
	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
