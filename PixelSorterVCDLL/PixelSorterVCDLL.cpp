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

JNIEXPORT jbyteArray JNICALL Java_pixelsorter_SortLib_sortImage
(JNIEnv *env, jobject obj, jint width, jint height, jbyteArray pixArr, jstring pathType, jint maxIntervals,
 jboolean randomizeIntervals, jint angle, jboolean toMerge, jboolean toReverse, jboolean toMirror,
 jboolean toInterval, jint lowThreshold)
{
    outImage = sf::Image{};
    sf::Image inputImage;

    //std::cout << "Start\n";

    int len = env->GetArrayLength(pixArr);

    //std::cout << "Got len\n";

    unsigned char* pixPtr = new unsigned char[len];

    //std::cout << "Allocated array\n";

    env->GetByteArrayRegion(pixArr, 0, len, reinterpret_cast<jbyte*>(pixPtr));

    //std::cout << "Filled array\n";

    const char *pathTypeCStr = env->GetStringUTFChars(pathType, 0);

    //std::cout << "C string created\n";

    //std::cout << len << "\n";

    inputImage.loadFromMemory(pixPtr, len);
    //inputImage.create(width, height, pixPtr);

    //inputImage.saveToFile(R"(D:\Egor\images\for_pixelsorting\test47.png)");

    //std::cout << "Input image created\n";

    Sorter srt = Sorter(&inputImage);
    outImage.create(width, height);
    outImage = srt.sort(pathTypeCStr, maxIntervals, randomizeIntervals, angle, toMerge, toReverse, toMirror,
                        toInterval, lowThreshold);

    //outImage.saveToFile(R"(D:\Egor\images\for_pixelsorting\test48.png)");

    //std::cout << "Out image created\n";

    env->ReleaseStringUTFChars(pathType, pathTypeCStr);

    //std::cout << "String released\n";

    jbyteArray newArr = env->NewByteArray(width * height * 4);

    //std::cout << "Out array allocated\n";

    env->SetByteArrayRegion(newArr, 0, width * height * 4, reinterpret_cast<const jbyte*>(outImage.getPixelsPtr()));

    //std::cout << "Out array filled\n";

    delete[] pixPtr;

    //std::cout << "Deleted pointer\n";

    return newArr;
}
