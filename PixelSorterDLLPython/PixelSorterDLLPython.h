#pragma once

#ifdef PIXELSORTERDLLPYTHON_EXPORTS
#define PIXELSORTERDLLPYTHON_API __declspec(dllexport)
#else
#define PIXELSORTERDLLPYTHON_API __declspec(dllimport)
#endif


extern "C" PIXELSORTERDLLPYTHON_API const unsigned char *sortImage(unsigned int width, unsigned int height, unsigned char *pixPtr,
                                                                   const char *pathType, int maxIntervals, bool randomizeIntervals,
                                                                   int angle, bool toMerge, bool toReverse, bool toMirror,
                                                                   bool toInterval, int lowThreshold, const char *funcType);
