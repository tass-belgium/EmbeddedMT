#include <vector>
#include <iostream>

#include "growRegions.hpp"
#include "findContours.hpp"


Contours::Contours(const region_t minimumRegionSize, const uint8_t maskWidthOneSide) :
    _minimumRegionSize(minimumRegionSize), _maskWidthOneSide(maskWidthOneSide) 
{
   ; 
}

GBL::Image_t Contours::andWithNeighbouringPixels(const GBL::Image_t image)
{
	GBL::Image_t resultImage = cv::Mat::zeros(image.rows, image.cols, image.type());
    // Mask:
    // 0 1 0
    // 1 1 1
    // 0 1 0
   
   // This can be vectorized	
    for(int32_t i = _maskWidthOneSide; i < image.rows - _maskWidthOneSide; ++i) {
		uint8_t* const resultRowPtr = resultImage.ptr<uint8_t>(i);
		const uint8_t* const imageRowPtr = image.ptr<uint8_t>(i);
       	for(int32_t j = _maskWidthOneSide; j < image.cols - _maskWidthOneSide; ++j) {
           uint8_t andedValue = imageRowPtr[j];
           // Apply the mask
           for(region_t k = 1; k <= _maskWidthOneSide; ++k) {
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

std::vector<std::vector<GBL::Point> > Contours::find(const GBL::Image_t& image) {
    // Assumption: the image was quantized where only one bit is non-zero. The non-zero bit defines to which quantization class the bit belongs
    // Step 1: And bits and all neighbouring bits. 
	GBL::Image_t tmpImage = andWithNeighbouringPixels(image);

    // Step 2: Grow uniform regions
    GrowRegions region(_minimumRegionSize, _maskWidthOneSide, 0x00);
    std::vector<std::vector<GBL::Point> > contours = region.growUniform(tmpImage);
    return contours;
}
