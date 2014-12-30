#ifndef AND_HPP
#define AND_HPP

#include <assert.h>

#include "cm/global.hpp"

namespace EmbeddedMT {
	namespace ImageProc {
		static inline GBL::Frame_t andWithNeighbouringPixels(const GBL::Frame_t& image, const uint8_t maskWidthOneSide) {
			GBL::Frame_t resultImage(image.rows, image.cols, image.type());
			// Mask:
			// 0 1 0
			// 1 1 1
			// 0 1 0
		   
		   // This can be vectorized	
			for(int32_t i = maskWidthOneSide; i < image.rows - maskWidthOneSide; ++i) {
				uint8_t* const resultRowPtr = resultImage.ptr<uint8_t>(i);
				const uint8_t* const imageRowPtr = image.ptr<uint8_t>(i);
				for(int32_t j = maskWidthOneSide; j < image.cols - maskWidthOneSide; ++j) {
				   uint8_t andedValue = imageRowPtr[j];
				   // Apply the mask
				   for(uint8_t k = 1; k <= maskWidthOneSide; ++k) {
					   // Horizontal displacement
						andedValue &= imageRowPtr[j-k] & imageRowPtr[j+k];
						// Vertical displacement
						const uint8_t* const imagePreviousRowPtr = image.ptr<uint8_t>(i-k);
						const uint8_t* const imageNextRowPtr = image.ptr<uint8_t>(i+k);
						andedValue &= imagePreviousRowPtr[j] & imageNextRowPtr[j];
				   }
				   resultRowPtr[j] = andedValue;
			   }
			}
			return resultImage;
		}

		namespace Meta {
			template<unsigned maskWidthOneSide>
			static inline GBL::Frame_t andWithNeighbouringPixels(const GBL::Frame_t& image) {
				GBL::Frame_t resultImage(image.rows, image.cols, image.type());
				// Mask:
				// 0 1 0
				// 1 1 1
				// 0 1 0
			   
			   // This can be vectorized	
				for(uint32_t i = maskWidthOneSide; i < image.rows - maskWidthOneSide; ++i) {
					uint8_t* const resultRowPtr = resultImage.ptr<uint8_t>(i);
					const uint8_t* const imageRowPtr = image.ptr<uint8_t>(i);
					for(uint32_t j = maskWidthOneSide; j < image.cols - maskWidthOneSide; ++j) {
					   uint8_t andedValue = imageRowPtr[j];
					   // Apply the mask
					   for(uint8_t k = 1; k <= maskWidthOneSide; ++k) {
						   // Horizontal displacement
							andedValue &= imageRowPtr[j-k] & imageRowPtr[j+k];
							// Vertical displacement
							const uint8_t* const imagePreviousRowPtr = image.ptr<uint8_t>(i-k);
							const uint8_t* const imageNextRowPtr = image.ptr<uint8_t>(i+k);
							andedValue &= imagePreviousRowPtr[j] & imageNextRowPtr[j];
					   }
					   resultRowPtr[j] = andedValue;
				   }
				}
				return resultImage;
			}

 			template <typename ValueType, unsigned valueSize, unsigned size>
			struct FitsInXHelper {
//				enum { value = ((valueSize >= size) ? 1 : 1 + FitsInXHelper<ValueType, valueSize + sizeof(ValueType), size>::value) }; 
				enum { value = ((valueSize >= size) ? 1 : 0 ) };
			};

			template <typename ValueType, unsigned size>
			struct FitsInX {
				enum { value = FitsInXHelper<ValueType, sizeof(ValueType), size>::value };
			};

			template <typename ValueType, typename VectorType, unsigned maskWidthOneSide>
			class VectorizedAnd {
				public:
					static GBL::Frame_t andWithNeighbouringPixels(const GBL::Frame_t& image) {
						GBL::Frame_t resultImage(image.rows, image.cols, image.type());

						// Make sure that the ValueType fits an integer number of times in the VectorType
						STATIC_ASSERT(sizeof(VectorType) % sizeof(ValueType) == 0, "The SIMD type should be an integer multiple of the actual type");
						const ValueType valueTypesPerVector = sizeof(VectorType)/sizeof(ValueType);
						const uint32_t startVectorIteration = FitsInX<VectorType, maskWidthOneSide*sizeof(ValueType)>::value;
						
						// Calculate part before we can start vectorizing stuff
						for(uint32_t i = maskWidthOneSide; i < startVectorIteration; ++i) {
							uint8_t* const resultRowPtr = resultImage.ptr<uint8_t>(i);
							const uint8_t* const imageRowPtr = image.ptr<uint8_t>(i);
							for(uint32_t j = maskWidthOneSide; j < image.cols - maskWidthOneSide; ++j) {
							   uint8_t andedValue = imageRowPtr[j];
							   // Apply the mask
							   for(uint8_t k = 1; k <= maskWidthOneSide; ++k) {
								   // Horizontal displacement
									andedValue &= imageRowPtr[j-k] & imageRowPtr[j+k];
									// Vertical displacement
									const uint8_t* const imagePreviousRowPtr = image.ptr<uint8_t>(i-k);
									const uint8_t* const imageNextRowPtr = image.ptr<uint8_t>(i+k);
									andedValue &= imagePreviousRowPtr[j] & imageNextRowPtr[j];
							   }
							   resultRowPtr[j] = andedValue;
						   }
						}
						return GBL::Frame_t::zeros(image.rows, image.cols, image.type());
					}
			};
		}
	}
}
#endif
