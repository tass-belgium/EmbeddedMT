/*
 * testLogging.cpp
 *
 *  Created on: Apr 11, 2014
 *      Author: cv
 */

#include "log/logging.hpp"

int main(int argc, char** argv) {
	LOG_ENTER("Entering main");
	LOG_INFO("Test no. %d", 1);
	LOG_ERROR("This is error no. %d", 2);
	LOG_WARNING("This is warning no. %d", 3);
	LOG_EXIT("Exiting main");
}
