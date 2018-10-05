/**
 *
 */

#pragma once

// STD, STL
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <algorithm>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

class DtstGenerator
{
public:
  void generate(std::vector<std::pair<cv::Point, cv::Point>>& b, cv::Mat& m, cv::Mat& m2);
private:

};
