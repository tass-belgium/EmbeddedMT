#ifndef AND_HPP
#define AND_HPP

#include <assert.h>

#include "cm/global.hpp"
#include "cm/meta.hpp"

namespace EmbeddedMT {
	namespace ImageProc {
		class And {
			public:
				static GBL::Frame_t fourNeighbourhood(const GBL::Frame_t& image, const uint8_t maskWidthOneSide) {
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
		};

		namespace Meta {
			template <typename ValueType, typename VectorType, unsigned maskWidthOneSide>
			class AndSimd {
				public:
					static GBL::Frame_t fourNeighbourhood(const GBL::Frame_t& image);
					static GBL::Frame_t verticalTwoNeighbourhood(const GBL::Frame_t& image);
					static GBL::Frame_t horizontalTwoNeighbourhood(const GBL::Frame_t& image);
				private:
					static void verticalTwoNeighbourhood(const GBL::Frame_t& image, GBL::Frame_t& resultImage);
					static void horizontalTwoNeighbourhood(const GBL::Frame_t& image, GBL::Frame_t& resultImage);
			};
		}

		// Template function definitions
		namespace Meta {
			template<typename ValueType, typename VectorType, unsigned maskWidthOneSide>
			GBL::Frame_t AndSimd<ValueType, VectorType, maskWidthOneSide>::fourNeighbourhood(const GBL::Frame_t& image) {
				GBL::Frame_t resultImage(image.rows, image.cols, image.type());

				// Hard to vectorize both masks into one. The vertical two neighbourhood is easier tough. So for now we split them out and loop over them separately	
				horizontalTwoNeighbourhood(image, resultImage);	

				// Apply vertical two neighbourhood
				verticalTwoNeighbourhood(image, resultImage);	
				return resultImage;
			}

			template<typename ValueType, typename VectorType, unsigned maskWidthOneSide>
			GBL::Frame_t AndSimd<ValueType, VectorType, maskWidthOneSide>::verticalTwoNeighbourhood(const GBL::Frame_t& image) {
				GBL::Frame_t resultImage(image.rows, image.cols, image.type());
				verticalTwoNeighbourhood(image, resultImage);
				return resultImage;
			}

			template<typename ValueType, typename VectorType, unsigned maskWidthOneSide>
			GBL::Frame_t AndSimd<ValueType, VectorType, maskWidthOneSide>::horizontalTwoNeighbourhood(const GBL::Frame_t& image) {
				GBL::Frame_t resultImage(image.rows, image.cols, image.type());
				horizontalTwoNeighbourhood(image, resultImage);
				return resultImage;
			}

			template<typename ValueType, typename VectorType, unsigned maskWidthOneSide>
			void AndSimd<ValueType, VectorType, maskWidthOneSide>::horizontalTwoNeighbourhood(const GBL::Frame_t& image, GBL::Frame_t& resultImage) {
				// We can not vectorize the horizontal two neighbourhood, so divide into two for now
				for(uint32_t i = maskWidthOneSide; i < image.rows - maskWidthOneSide; ++i) {
					uint8_t* const resultRowPtr = resultImage.ptr<uint8_t>(i);
					const uint8_t* const imageRowPtr = image.ptr<uint8_t>(i);
					for(uint32_t j = maskWidthOneSide; j < image.cols - maskWidthOneSide; ++j) {
					   ValueType andedValue = imageRowPtr[j];
					   // Apply the mask
					   for(uint8_t k = 1; k <= maskWidthOneSide; ++k) {
						   // Horizontal displacement
							andedValue &= imageRowPtr[j-k] & imageRowPtr[j+k];
					   }
					   resultRowPtr[j] = andedValue;
				   }
				}
			}

			template<typename ValueType, typename VectorType, unsigned maskWidthOneSide>
			void AndSimd<ValueType, VectorType, maskWidthOneSide>::verticalTwoNeighbourhood(const GBL::Frame_t& image, GBL::Frame_t& resultImage) {
				// ------------------------------ Assert input -----------------------------------
				// Make sure that the ValueType fits an integer number of times in the VectorType
				STATIC_ASSERT(sizeof(VectorType) % sizeof(ValueType) == 0, "The SIMD type should be an integer multiple of the actual type");
				// ------------------------------ Compilation time calculations ---------------------------
				const ValueType valueTypesPerVector = sizeof(VectorType)/sizeof(ValueType);
				const uint32_t startValueIteration = Utils::Meta::FitsInX<VectorType, maskWidthOneSide*sizeof(ValueType)>::value;
				const uint32_t startVectorIteration = startValueIteration/valueTypesPerVector;

				// ------------------------------ Run time -------------------------
				const uint32_t endVectorIteration = (image.rows - startValueIteration) / valueTypesPerVector;
				const uint32_t endValueIteration = (image.rows - startValueIteration) % valueTypesPerVector;

				for(uint32_t row = maskWidthOneSide; row < image.rows - maskWidthOneSide; ++row) {
					// Vertical displacement for beginning
					ValueType* const resultPtrBegin = resultImage.ptr<ValueType>(row);
					for(uint8_t k = 1; k <= maskWidthOneSide; ++k) {
						const ValueType* const imagePreviousRowPtr = image.ptr<ValueType>(row-k);
						const ValueType* const imageNextRowPtr = image.ptr<ValueType>(row+k);
						for(uint32_t col = maskWidthOneSide; col < startValueIteration; col++) {
							resultPtrBegin[col] &= imagePreviousRowPtr[col] & imageNextRowPtr[col];
						}
					}

					VectorType* const resultPtr = resultImage.ptr<VectorType>(row);
					for(uint32_t col = startVectorIteration; col <	endVectorIteration; ++col) {
						for(uint8_t k = 1; k <= maskWidthOneSide; ++k) {
							const VectorType* const imagePreviousRowPtr = image.ptr<VectorType>(row-k);
							const VectorType* const imageNextRowPtr = image.ptr<VectorType>(row+k);
							// Vertical displacement
							resultPtr[col] &= imagePreviousRowPtr[col] & imageNextRowPtr[col];
						}
					}

					// Vertical displacement for the end of the col that does not fit in VectorType
					ValueType* const resultPtr2 = reinterpret_cast<ValueType* const>(resultPtr);
					for(uint8_t k = 1; k <= maskWidthOneSide; ++k) {
						const ValueType* const imagePreviousRowPtr = image.ptr<ValueType>(row-k);
						const ValueType* const imageNextRowPtr = image.ptr<ValueType>(row+k);
						for(uint32_t col = image.cols - maskWidthOneSide - endValueIteration; col < image.cols - maskWidthOneSide; ++col) {
							resultPtr2[col] &= imagePreviousRowPtr[col] & imageNextRowPtr[col];
						}
					}
				}
			}
		}
	}
}
#endif
