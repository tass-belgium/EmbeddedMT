#include "bufferOutput.hpp"

using std::string;

namespace EmbeddedMT {
	namespace OutputMethod {
		BufferOutput::BufferOutput() : _buffer() {
			;
		}

		BufferOutput::BufferOutput(const uint32_t initialBufferSize) : _buffer() {
			_buffer.reserve(initialBufferSize);
		}

		BufferOutput::~BufferOutput() {
			;
		}

		GBL::CmRetCode_t BufferOutput::open(const char* filename) {
			return GBL::RESULT_SUCCESS;
		}

		GBL::CmRetCode_t BufferOutput::write(const std::string& message) {
			_buffer.push_back(message);
			return GBL::RESULT_SUCCESS;
		}

		GBL::CmRetCode_t BufferOutput::close() {
			return GBL::RESULT_SUCCESS;
		}
			
		size_t BufferOutput::size(void) {
			return _buffer.size();
		}
				
		string BufferOutput::getLatest(void) {
			if(size() == 0) {
				return string("");
			}
			string lastMessage = _buffer.back();
			_buffer.pop_back();
			return lastMessage;
		}
	}
}


