/*
 * =====================================================================================
 *
 *       Filename:  socketInterface.cpp
 *
 *    Description:  Socket interface implementation
 *
 *        Version:  1.0
 *        Created:  04/28/2014 12:10:32 PM
 *       Revision:  none
 *
 *         Author:  Bart Verhagen (bv), bart.verhagen@tass.be
 *   Organization:  TASS
 *
 * =====================================================================================
 */

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>

#include "socketInterface.hpp"

namespace OutputMethod {

SocketInterface::SocketInterface(uint16_t portNo) : _portNo(portNo) {
	;
}

GBL::CmRetCode_t SocketInterface::open(const char* filename) {
	LOG_ENTER("ip address = %s, port = %d", filename, _portNo);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	_sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(_sockfd >= 0) {
		struct sockaddr_in servAddress;
		servAddress.sin_family = AF_INET;
		servAddress.sin_addr.s_addr = inet_addr(filename);
		servAddress.sin_port = htons(_portNo);

		/*  Establish connection */
		if (connect(_sockfd, (struct sockaddr *) &servAddress, sizeof(servAddress)) < 0) {
			LOG_ERROR("Could not connect to socket");
		} else {
			result = GBL::RESULT_SUCCESS;
		}
	} else {
		LOG_ERROR("Error opening socket");
	}
	LOG_EXIT("result = %d", result);
	return result;
}

GBL::CmRetCode_t SocketInterface::write(const GBL::Displacement_t& displacement) {
	LOG_ENTER("Displacement = %p", &displacement);
	GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
	char message[62];
	snprintf(message, sizeof(message), "{\"sequenceNo\":%4d,\"displacementX\":%4d,\"displacementY\":%4d}", displacement.sequenceNo, displacement.x, displacement.y);
	if (send(_sockfd, message, sizeof(message), 0) != sizeof(message)) {
		LOG_ERROR("Could not send message");
		result = GBL::RESULT_FAILURE;
	}
	LOG_EXIT("result = %d", result);
	return result;
}

GBL::CmRetCode_t SocketInterface::write(const GBL::Frame_t frame) {
	return GBL::RESULT_FAILURE;
}

GBL::CmRetCode_t SocketInterface::close() {
	LOG_ENTER("void");
	::close(_sockfd);
	LOG_EXIT("GBL::RESULT_SUCCESS");
	return GBL::RESULT_SUCCESS;
}

}
