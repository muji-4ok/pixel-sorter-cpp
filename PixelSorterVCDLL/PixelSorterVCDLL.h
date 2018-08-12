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
#include <jni.h>

#ifndef _Included_pixelsorter_SortLib
#define _Included_pixelsorter_SortLib
#ifdef __cplusplus
extern "C" {
    #endif

    /*
    * Class:     pixelsorter_SortLib
    * Method:    sortImage
    * Signature: (II[BLjava/lang/String;IZIZZZZI)[B
    */
    JNIEXPORT jbyteArray JNICALL Java_pixelsorter_SortLib_sortImage
    (JNIEnv *, jobject, jint, jint, jbyteArray, jstring, jint, jboolean, jint, jboolean, jboolean, jboolean, jboolean, jint);

    #ifdef __cplusplus
}
#endif
#endif