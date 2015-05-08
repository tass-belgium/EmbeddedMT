#include "recognizeGestures.hpp"

#include "log/logging.hpp"

namespace EmbeddedMT {
	namespace OutputMethod {
		RecognizeGestures::RecognizeGestures(const Backend::ControlInterface* const backend) : _backend(*backend), _magnitudeThreshold(15U) {
			;
		}

		GBL::CmRetCode_t RecognizeGestures::write(const GBL::Displacement_t& displacement) {
			LOG_INFO("Displacement: x = %d, y = %d", displacement.x, displacement.y);
			static uint8_t consecutiveDisplacementsInThatDirection = 0U;
			static Direction previousDirection = Direction_right;
			uint16_t magnitude;
			Direction direction = calculateDirection(displacement, magnitude);
			LOG_INFO("Direction = %d", direction);
			LOG_INFO("Consecutive directions = %d", consecutiveDisplacementsInThatDirection);
			if(direction == previousDirection) {
				++consecutiveDisplacementsInThatDirection;
				if(consecutiveDisplacementsInThatDirection > 3U) {
					if(magnitude > _magnitudeThreshold) {
						switch(direction) {
							case Direction_right:
								_backend.moveRight();
								break;
							case Direction_left:
								_backend.moveLeft();
								break;
							case Direction_up:
								_backend.moveUp();
								break;
							case Direction_down:
								_backend.moveDown();
								break;
							default:
								LOG_ERROR("Error: unknown direction");
						}
					}
				}
			} else {
				consecutiveDisplacementsInThatDirection = 0U;
			}
			previousDirection = direction;
			return GBL::RESULT_SUCCESS;
		}

		Direction RecognizeGestures::calculateDirection(const GBL::Displacement_t& displacement, uint16_t& magnitude) {
			if(displacement.x > displacement.y) {
				if(displacement.x >= 0) {
					magnitude = displacement.x;
					return Direction_right;
				} else {
					magnitude = -displacement.x;
					return Direction_left;
				}
			} else {
				if(displacement.y >= 0) {
					magnitude = displacement.y;
					return Direction_up;
				} else {
					magnitude = -displacement.y;
					return Direction_down;
				}
			}
		}
	}
}
