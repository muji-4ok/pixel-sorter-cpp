#pragma once
#ifndef SORTER_H
#define SORTER_H
#include <SFML/Graphics.hpp>
#include <vector>
#include <algorithm>
#include <iterator>
#include <string>
#include <iostream>
#include <stdlib.h>
#include <cmath>

struct Point
{
    unsigned short int i;
    unsigned short int j;
    static sf::Image *image;

    Point(unsigned short int ii, unsigned short int jj)
    {
        i = ii;
        j = jj;
    }

    bool operator< (const Point& other) const
    {
        auto c1 = image->getPixel(j, i);
        auto c2 = image->getPixel(other.j, other.i);
        unsigned short int lightness1 = c1.r + c1.g + c1.b;
        unsigned short int lightness2 = c2.r + c2.g + c2.b;

        return lightness1 < lightness2;
    }
};

class Sorter
{
public:
    Sorter(sf::Image* img);
    virtual ~Sorter();
    sf::Image sort(std::string pathType, std::string funcType, int maxIntervals, bool randomizeIntervals, int angle, bool toMerge);

private:
    unsigned int width;
    unsigned int height;
    sf::Image* image;
    sf::Image sortedImage;

    std::vector<std::vector<Point>> rows();
    std::vector<std::vector<Point>> columns();
    std::vector<std::vector<Point>> rectangles();
    std::vector<std::vector<Point>> angled(int angle);

    std::vector<std::vector<Point>> applyIntervals(std::vector<std::vector<Point>>* path, int maxIntervals, bool randomize);
    void mergeIntoOne(std::vector<std::vector<Point>>* path);
};

#endif // SORTER_H
