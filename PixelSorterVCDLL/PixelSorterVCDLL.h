// PixelSorterVCDLL.h

#pragma once

#ifdef PIXELSORTERVCDLL_EXPORTS
#define PIXELSORTERVCDLL_API __declspec(dllexport)
#else
#define PIXELSORTERVCDLL_API __declspec(dllimport)
#endif


extern "C" PIXELSORTERVCDLL_API const unsigned char *sortImage(unsigned int width, unsigned int height, unsigned char *pixPtr,
                                                               const char *pathType, int maxIntervals, bool randomizeIntervals,
                                                               int angle, bool toMerge, bool toReverse, bool toMirror,
                                                               bool toInterval, int lowThreshold);
