// PixelSorterVCDLL.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "PixelSorterVCDLL.h"
#include "Sorter.h"

sf::Image outImage;

PIXELSORTERVCDLL_API const unsigned char * sortImage(unsigned int width, unsigned int height, unsigned char *pixPtr,
                                                     const char *pathType, int maxIntervals, bool randomizeIntervals,
                                                     int angle, bool toMerge, bool toReverse, bool toMirror,
                                                     bool toInterval, int lowThreshold)
{
    outImage = sf::Image{};
    sf::Image inputImage;
    inputImage.create(width, height, pixPtr);

    Sorter srt = Sorter(&inputImage);
    outImage.create(width, height);
    outImage = srt.sort(pathType, maxIntervals, randomizeIntervals, angle, toMerge, toReverse, toMirror,
                        toInterval, lowThreshold);

    return outImage.getPixelsPtr();
}
