/**
 *
 */

#pragma once

// STD, STL
#include <iostream>
#include <vector>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/** @brief */
using coords     = std::pair<cv::Point, cv::Point>;
/** @brief */
using ROI_buffer = std::vector<coords>;


void mouseClick(int event, int x, int y, int flags, void *param);

ROI_buffer getRegionsOfInterest(cv::Mat& m);
