#pragma once


#include <SFML/Graphics.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <vector>
#include <deque>
#include <algorithm>
#include <iterator>
#include <cstdlib>
#include <cmath>

enum FuncType
{
    LIGHTNESS = 0,
    VALUE = 1,
    SATURATION = 2,
    HUE
};

struct Point
{
    unsigned short int i;
    unsigned short int j;
    static sf::Image *image;
    static FuncType funcType;

    Point(unsigned short int i, unsigned short int j);

    unsigned short getMin(const sf::Color& color) const;

    unsigned short getMax(const sf::Color& color) const;

    float getMinFloat(const float& r, const float& g, const float& b) const;

    float getMaxFloat(const float& r, const float& g, const float& b) const;

    float getSaturation(const sf::Color& color) const;

    float getHue(const sf::Color& color) const;

    bool operator< (const Point& other) const;
};

class Sorter
{
public:
    Sorter(sf::Image* img);
    sf::Image sort(const char *pathType, int maxIntervals, bool randomizeIntervals,
                   int angle, bool toMerge, bool toReverse, bool toMirror,
                   bool toInterval, int lowThreshold, const char *funcType);

private:
    unsigned int width;
    unsigned int height;
    sf::Image* image;

    std::vector<std::vector<Point>> rows();
    std::vector<std::vector<Point>> columns();
    std::vector<std::vector<Point>> rectangles();
    std::vector<std::vector<Point>> angled(int angle);

    std::vector<std::vector<Point>> applyIntervals(std::vector<std::vector<Point>>* path, int maxIntervals, bool randomize);
    void mergeIntoOne(std::vector<std::vector<Point>>* path);
    void mirror(std::vector<std::vector<Point>>* path);
    void reverseSort(std::vector<std::vector<Point>>* path);

    cv::Mat getEdges(int lowThreshold, int highThreshold, int kernelSize);
    std::vector<std::vector<Point>> edgesRows(int lowThreshold, int highThreshold, int kernelSize);
};
