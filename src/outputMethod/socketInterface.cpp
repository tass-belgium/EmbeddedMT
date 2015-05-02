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
#include <unistd.h>

#include "socketInterface.hpp"

using std::string;

namespace EmbeddedMT {
	namespace OutputMethod {
		SocketInterface::SocketInterface(const std::string& host, uint16_t portNo) {
			open(host, portNo);
		}

		void SocketInterface::open(const std::string& host, uint16_t portNo) {
			LOG_ENTER("ip address = %s, port = %d", host.c_str(), portNo);
			_sockfd = socket(AF_INET, SOCK_STREAM, 0);
			if(_sockfd >= 0) {
				struct sockaddr_in servAddress;
				servAddress.sin_family = AF_INET;
				servAddress.sin_addr.s_addr = inet_addr(host.c_str());
				servAddress.sin_port = htons(portNo);

				/*  Establish connection */
				if (connect(_sockfd, (struct sockaddr *) &servAddress, sizeof(servAddress)) < 0) {
					LOG_ERROR("Could not connect to socket");
				}
			} else {
				LOG_ERROR("Error opening socket");
			}
		}

		GBL::CmRetCode_t SocketInterface::write(const GBL::Displacement_t& displacement) {
			LOG_ENTER("Displacement = %p", &displacement);
			GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
			char message[62];
			snprintf(message, sizeof(message), "{\"sequenceNo\":%4d,\"displacementX\":%4d,\"displacementY\":%4d}", displacement.sequenceNo, displacement.x, displacement.y);
			if (send(_sockfd, message, sizeof(message), 0) != sizeof(message)) {
				LOG_ERROR("Could not send message");
			} else {
				result = GBL::RESULT_SUCCESS;
			}
			
			LOG_EXIT("result = %d", result);
			return result;
		}

		GBL::CmRetCode_t SocketInterface::write(const string& msg) {
			LOG_ENTER("Sending message '%s'", msg.c_str());
			GBL::CmRetCode_t result = GBL::RESULT_FAILURE;
 			if (send(_sockfd, msg.c_str(), msg.size(), 0) != uint32_t(msg.size())) {
				LOG_ERROR("Could not send message");
			} else {
				result = GBL::RESULT_SUCCESS;
			}
			LOG_EXIT("result = %d", result);
			return result;
		}

		std::string SocketInterface::recv(uint32_t bytes) {
			std::string output;
			output.resize(bytes);

			int bytes_received = read(_sockfd, &output[0], bytes-1);
			if (bytes_received<0) {
				std::cerr << "Failed to read data from socket.\n";
				return "";
			}

			output[bytes_received] = 0;
			return output;
		}

		SocketInterface::~SocketInterface(void) {
			LOG_ENTER("Destroying socket interface");
			::close(_sockfd);
			LOG_EXIT("Done");
		}
	}
}
