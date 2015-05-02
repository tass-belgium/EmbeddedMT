/*
 * outputImageSequence.hpp
 *
 *  Created on: Apr 18, 2014
 *      Author: cv
 */

#ifndef OUTPUTIMAGESEQUENCE_HPP_
#define OUTPUTIMAGESEQUENCE_HPP_

#include "outputMethodInterface.hpp"

namespace EmbeddedMT {
	namespace OutputMethod {
		class OutputImageSequence : public OutputMethodFrameInterface {
		public:
			OutputImageSequence(const std::string& filename);
			virtual GBL::CmRetCode_t write(const GBL::Frame_t& frame);

		private:
			// Disable default constructor
			OutputImageSequence(void);

			uint32_t _imageIndex;
			std::string _filename;
		};
	}
}

#endif /* OUTPUTIMAGESEQUENCE_HPP_ */
