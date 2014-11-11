#ifndef FINDCONTOURS_HPP
#define FINDCONTOURS_HPP

#include <vector>
#include "cm/global.hpp"

class Contours {
    public:
        Contours(const uint8_t* image, const uint32_t sizeX, const uint32_t sizeY);
        std::vector<std::vector<GBL::Point> > find(void);


    private:
        Contours(void);

        void andWithNeighbouringPixels(uint8_t* const resultImage);

        const uint8_t* _image;
        const uint32_t _sizeX;
        const uint32_t _sizeY;
};
#endif
