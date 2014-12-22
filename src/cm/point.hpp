#ifndef POINT_HPP
#define POINT_HPP

#include <stdint.h>
#include <iostream>

namespace EmbeddedMT {
	namespace GBL {
		class Point
		{
			public:
				Point();
				Point(const uint32_t x, const uint32_t y);

				inline uint32_t getX() const {
					return _x;
				}

				inline uint32_t getY() const {
					return _y;
				}
				
				inline uint32_t getRow() const {
					return getY();
				}

				inline uint32_t getCol() const {
					return getX();
				}

				inline void setX(const uint32_t x) {
					_x = x;   
				}

				inline void setY(const uint32_t y) {
					_y = y;
				}

				inline void setRow(const uint32_t row) {
					setY(row);
				}

				inline void setCol(const uint32_t col) {
					setX(col);
				}

				inline void incrementX(const uint32_t xIncrement) {
					_x += xIncrement;   
				}

				inline void incrementY(const uint32_t yIncrement) {
					_y += yIncrement;   
				}

				friend std::ostream& operator<<(std::ostream& os, const Point& point) {
					return os << "[" << point.getX() << ", " << point.getY() << "]";
				}

			private:
				uint32_t _x;
				uint32_t _y;
		};
	}
}

#endif
