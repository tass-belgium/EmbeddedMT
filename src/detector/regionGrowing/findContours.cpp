#include <vector>
#include <iostream>

#include "growRegions.hpp"
#include "findContours.hpp"

const region_t MINIMUM_REGION_SIZE = 2;
const uint8_t MASK_WIDTH_ONE_SIDE = 1;

Contours::Contours(const uint8_t* image, const uint32_t sizeX, const uint32_t sizeY) :
    _image(image), _sizeX(sizeX), _sizeY(sizeY) 
{
    ;
}

void Contours::andWithNeighbouringPixels(uint8_t* const resultImage)
{
    // Mask:
    // 0 1 0
    // 1 1 1
    // 0 1 0
    
    // Fill border with 0's
    for(uint32_t i = 0; i < _sizeY; i++) {
        for(uint32_t j = 0; j < MASK_WIDTH_ONE_SIDE; j++) {
            resultImage[i * _sizeX + j] = 0;
            resultImage[i * _sizeX + _sizeX - 1 - j] = 0;
        }
    }
    for(uint32_t i = 0; i < MASK_WIDTH_ONE_SIDE; i++) {
        for(uint32_t j = 0; j < _sizeX; j++) {
            resultImage[i * _sizeX + j] = 0;
            resultImage[(_sizeY - i - 1)* _sizeX + j] = 0;
        }
    }

    for(uint32_t i = MASK_WIDTH_ONE_SIDE; i < _sizeY - MASK_WIDTH_ONE_SIDE; i++) {
       for(uint32_t j = MASK_WIDTH_ONE_SIDE; j < _sizeX - MASK_WIDTH_ONE_SIDE; j++) {
           uint8_t andedValue = _image[i * _sizeX + j];
           // Apply the mask
           for(region_t k = 1; k <= MASK_WIDTH_ONE_SIDE; k++) {
               // Horizontal displacement
                andedValue &= _image[i * _sizeX + j - k] & _image[i * _sizeX + j + k];
                // Vertical displacement
                andedValue &= _image[(i - k) * _sizeX + j] & _image[(i+k) * _sizeX + j];
           }
            resultImage[i * (_sizeX*MASK_WIDTH_ONE_SIDE) + j] = andedValue;
       }
    }
}

std::vector<std::vector<GBL::Point> > Contours::find() {
    // Assumption: the image was quantized where only one bit is non-zero. The non-zero bit defines to which quantization class the bit belongs
    // Step 1: And bits and all neighbouring bits. 
    uint8_t tmpImage[_sizeX*_sizeY];
    andWithNeighbouringPixels(tmpImage);

    // Step 2: Grow uniform regions
    GrowRegions region(MINIMUM_REGION_SIZE, MASK_WIDTH_ONE_SIDE, 0x01);
    std::vector<std::vector<GBL::Point> > contours = region.growUniform(tmpImage, _sizeX, _sizeY);
    return contours;
}
