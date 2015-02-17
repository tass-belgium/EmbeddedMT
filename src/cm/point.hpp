#ifndef POINT_HPP
#define POINT_HPP

#include <stdint.h>
#include <iostream>
#include "cm/global.hpp"

namespace EmbeddedMT {
	namespace GBL {
		class Point
		{
			// Use implicit inlines
			public:
				Point() : _x(0), _y(0) {
					;
				}

				Point(const uint32_t x, const uint32_t y) : _x(x), _y(y) {
					;
				}

				uint32_t getX() const NOEXCEPT {
					return _x;
				}

				uint32_t getY() const NOEXCEPT {
					return _y;
				}
				
				uint32_t getRow() const NOEXCEPT_EXPR(getY()) {
					return getY();
				}

				uint32_t getCol() const NOEXCEPT_EXPR(getX()) {
					return getX();
				}

				void setX(const uint32_t x) NOEXCEPT {
					_x = x;   
				}

				void setY(const uint32_t y) NOEXCEPT {
					_y = y;
				}

				void setRow(const uint32_t row) NOEXCEPT_EXPR(setY()) {
					setY(row);
				}

				void setCol(const uint32_t col) NOEXCEPT_EXPR(setX()) {
					setX(col);
				}

				void incrementX(const uint32_t xIncrement) NOEXCEPT {
					_x += xIncrement;   
				}

				void incrementY(const uint32_t yIncrement) NOEXCEPT {
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
