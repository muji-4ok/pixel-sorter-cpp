// PixelSorterDLLPython.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "PixelSorterDLLPython.h"
#include "Sorter.h"

sf::Image outImage;

PIXELSORTERDLLPYTHON_API const unsigned char * sortImage(unsigned int width, unsigned int height, unsigned char *pixPtr,
                                                         const char *pathType, int maxIntervals, bool randomizeIntervals,
                                                         int angle, bool toMerge, bool toReverse, bool toMirror,
                                                         bool toInterval, int lowThreshold, const char *funcType)
{
    outImage = sf::Image{};
    sf::Image inputImage;
    inputImage.create(width, height, pixPtr);

    Sorter srt = Sorter(&inputImage);
    outImage.create(width, height);
    outImage = srt.sort(pathType, maxIntervals, randomizeIntervals, angle, toMerge, toReverse, toMirror,
                        toInterval, lowThreshold, funcType);

    return outImage.getPixelsPtr();
}
