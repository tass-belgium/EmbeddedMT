/*
 * socketInterface.hpp
 *
 *  Created on: Apr 28, 2014
 *      Author: Bart Verhagen
 */

#ifndef SOCKETINTERFACE_HPP_
#define SOCKETINTERFACE_HPP_

#include <string>

#include "cm/global.hpp"
#include "outputMethodInterface.hpp"

namespace EmbeddedMT {
	namespace OutputMethod {
		class SocketInterface : public OutputMethodStringInterface, public OutputMethodDisplacementInterface {
		public:
			SocketInterface(const std::string& host, uint16_t portNo);
			~SocketInterface(void);

			virtual GBL::CmRetCode_t write(const GBL::Displacement_t& displacement);
			virtual GBL::CmRetCode_t write(const std::string& msg);

			std::string recv(uint32_t bytes);
			std::string listen(void);
		private:
			// Disable default constructor
			SocketInterface(void);
			void open(const std::string& host, uint16_t portNo);

			int32_t _sockfd;
		};
	}
}

#endif /* SOCKETINTERFACE_HPP_ */
