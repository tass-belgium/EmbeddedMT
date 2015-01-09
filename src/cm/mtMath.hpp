#ifndef MTMATH_HPP
#define MTMATH_HPP

namespace EmbeddedMT {
	namespace Math {
		class Math {
			public:
				static inline uint32_t log2(uint32_t value) {
					int32_t result = -1;
					while (value != 0) {
						++result;
						value >>= 1;
					}
					return static_cast<uint32_t>(result);
				}
		};
	}
}

#endif
