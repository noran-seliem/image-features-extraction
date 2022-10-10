#pragma once
#ifndef HOUGH_H_
#define HOUGH_H_

#include <opencv2/opencv.hpp>
#include <vector>
#include <iostream>

#define DEG2RAD 0.017453293f

using namespace cv;
using namespace std;

static unsigned int* Accu;
static int AccuWidth;
static int AccuHight;
static int ImageWidth;
static int ImageHight;

static Mat OriginalImage, GrayImage, BlurredImage, EdgesImage, AccumulatorImage, ResultImage;

int HoughTransform(unsigned char* img_data, int w, int h);
std::vector< std::pair< std::pair<int, int>, std::pair<int, int> > > GetLines(int threshold);
const unsigned int* GetAccu(int* w, int* h);
void doTransform(string ImagePath, int threshold);

#endif /* HOUGH_H_ */