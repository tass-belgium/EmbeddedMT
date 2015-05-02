/*
 * kodi.hpp
 *
 *  Created on: Apr 27, 2015
 *      Author: Bart Verhagen
 */

#ifndef KODI_HPP_
#define KODI_HPP_

#include <string>
#include <map>

#include "outputMethodInterface.hpp"
#include "socketInterface.hpp"

namespace EmbeddedMT {
	namespace OutputMethod {
		typedef std::map<std::string, std::string> KodiParameterList;

		class Kodi {
		public:
			Kodi(OutputMethodStringInterface& outputMethod);

			void inputRight(void);
			void inputLeft(void);
			void inputDown(void);
			void inputUp(void);
		private:
			OutputMethodStringInterface& _outputMethod;
			void write(const std::string& method, const std::string& id = "1");
			void write(const std::string& method, const KodiParameterList& parameters, const std::string& id = "1");
			void writeMessage(const std::string& msg);
		};
	}
}

#endif /* KODI_HPP_ */
