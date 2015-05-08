#ifndef RECOGNIZEGESTURES_HPP
#define RECOGNIZEGESTURES_HPP

#include "outputMethodInterface.hpp"

#include "backend/controlInterface.hpp"

namespace EmbeddedMT {
	namespace OutputMethod {
		enum Gesture {
			Gesture_right,
			Gesture_left,
			Gesture_up,
			Gesture_down,
		};

		enum Direction {
			Direction_right,
			Direction_left,
			Direction_up,
			Direction_down
		};

		class RecognizeGestures : public OutputMethodDisplacementInterface {
			public:
				RecognizeGestures(const Backend::ControlInterface* const backend);
				virtual GBL::CmRetCode_t write(const GBL::Displacement_t& displacement);
			private:
				static Direction calculateDirection(const GBL::Displacement_t& displacement, uint16_t& magnitude);

				const Backend::ControlInterface& _backend;
				uint16_t _magnitudeThreshold;
		};
	}
}

#endif
