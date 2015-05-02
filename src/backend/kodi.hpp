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

#include "outputMethod/outputMethodInterface.hpp"

#include "controlInterface.hpp"

namespace EmbeddedMT {
	namespace Backend {
		typedef std::map<std::string, std::string> KodiParameterList;

		class Kodi : public ControlInterface {
		public:
			Kodi(OutputMethod::OutputMethodStringInterface& outputMethod);

			virtual void moveRight(void) const;
			virtual void moveLeft(void) const;
			virtual void moveDown(void) const;
			virtual void moveUp(void) const;
		private:
			OutputMethod::OutputMethodStringInterface& _outputMethod;
			void write(const std::string& method, const std::string& id = "1") const;
			void write(const std::string& method, const KodiParameterList& parameters, const std::string& id = "1") const;
			void writeMessage(const std::string& msg) const;
		};
	}
}

#endif /* KODI_HPP_ */
