#ifndef BUFFEROUTPUT_HPP
#define BUFFEROUTPUT_HPP

#include <vector>
#include <string>

#include "outputMethodInterface.hpp"

namespace EmbeddedMT {
	namespace OutputMethod {
		class BufferOutput : public OutputMethodStringInterface {
			public:
				BufferOutput();
				BufferOutput(const uint32_t initialBufferSize);
				virtual ~BufferOutput();
				virtual GBL::CmRetCode_t write(const std::string& message);
				
				size_t size(void);
				std::string getLatest(void);
			private:
				std::vector<std::string> _buffer;
		};
	}
}

#endif
