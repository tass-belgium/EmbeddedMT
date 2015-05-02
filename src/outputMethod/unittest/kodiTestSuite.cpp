/*
 * =====================================================================================
 *
 *       Filename:  imageProcInterfaceTestSuite.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/29/2014 08:06:13 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Bart Verhagen (bv), bart.verhagen@tass.be
 *   Organization:  TASS
 *
 * =====================================================================================
 */
#include <iostream>
#include <sstream>
#include <string>

#include "catch.hpp"

#include "cm/global.hpp"

#include "outputMethod/kodi.hpp"
#include "outputMethod/socketInterface.hpp"
#include "outputMethod/bufferOutput.hpp"

using EmbeddedMT::OutputMethod::Kodi;
using EmbeddedMT::OutputMethod::BufferOutput;
using std::string;
using std::stringstream;

namespace EmbeddedMT {
	namespace test {
		std::string getJsonRpcCall(const std::string& method) {
			stringstream msg;
			msg << "{\"jsonrpc\":\"2.0\",\"method\":\"" << method <<"\",\"id\":1}";
			return msg.str();
		}

		SCENARIO("We do some basic request on the Kodi API") {
			GIVEN("A kodi usage object and a port to listen on") {
				BufferOutput outputStub;
				Kodi kodi(outputStub);

				WHEN("We move the cursor to the right") {
					kodi.inputRight();
					THEN("We should receive the right command") {
						REQUIRE(outputStub.size() == 1U);
						REQUIRE(outputStub.getLatest() == getJsonRpcCall("Input.Right"));
						REQUIRE(outputStub.size() == 0U);
					}
				}
				WHEN("We move the cursor to the left") {
					kodi.inputLeft();
					THEN("We should get the command") {
						REQUIRE(outputStub.size() == 1U);
						REQUIRE(outputStub.getLatest() == getJsonRpcCall("Input.Left"));
						REQUIRE(outputStub.size() == 0U);
					}
				}
				WHEN("We move the cursor up") {
					kodi.inputUp();
					THEN("We should get the command") {
						REQUIRE(outputStub.size() == 1U);
						REQUIRE(outputStub.getLatest() == getJsonRpcCall("Input.Up"));
						REQUIRE(outputStub.size() == 0U);
					}
				}
				WHEN("We move the cursor down") {
					kodi.inputDown();
					THEN("We should get the command") {
						REQUIRE(outputStub.size() == 1U);
						REQUIRE(outputStub.getLatest() == getJsonRpcCall("Input.Down"));
						REQUIRE(outputStub.size() == 0U);
					}
				}
			}
		}
	}
}
