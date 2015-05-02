/*
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
#include <sstream>
#include <cassert>

#include "cm/global.hpp"
#include "log/logging.hpp"

#include "kodi.hpp"

using std::string;
using std::stringstream;

namespace EmbeddedMT {
	namespace OutputMethod {
		Kodi::Kodi(OutputMethodStringInterface& outputMethod) : _outputMethod(outputMethod) {
			;
		}

		void Kodi::write(const std::string& method, const std::string& id) {
			KodiParameterList emptyList;
			write(method, emptyList, id);
		}

		void Kodi::write(const std::string& method, const KodiParameterList& parameters, const std::string& id) {
			LOG_ENTER("Writing method = %s", method.c_str());
			stringstream msg;
			msg << "{\"jsonrpc\":\"2.0\",\"method\":\"" << method <<"\",\"id\":" << id << "}";
			if(parameters.size() > 0) {
				msg << "\"params\":{";	
				for(KodiParameterList::const_iterator it = parameters.begin(); it != parameters.end(); ++it) {
					msg << "\"" << it->first << "\":" << it->second;	
				}
				msg << "}";
			}
			writeMessage(msg.str());
			LOG_EXIT("Done");
		}

		void Kodi::inputRight() {
			LOG_ENTER("Sending input right signal");
			write("Input.Right");
			LOG_EXIT("Done");
		}

		void Kodi::inputLeft() {
			LOG_ENTER("Sending input left signal");
			write("Input.Left");
			LOG_EXIT("Done");
		}

		void Kodi::inputUp() {
			LOG_ENTER("Sending input up signal");
			write("Input.Up");
			LOG_EXIT("Done");
		}

		void Kodi::inputDown() {
			LOG_ENTER("Sending input down signal");
			write("Input.Down");
			LOG_EXIT("Done");
		}

		void Kodi::writeMessage(const string& msg) {
			LOG_ENTER("Writing '%s'", msg.c_str());
			GBL::CmRetCode_t result = _outputMethod.write(msg); 
			if(result != GBL::RESULT_SUCCESS) {
				LOG_ERROR("An error occurred while sending the message %s", msg.c_str());
			}
			LOG_EXIT("result = %i", result);
		}
	}
}
