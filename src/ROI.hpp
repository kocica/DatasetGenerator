/**
 * @file    ROI.hpp
 * @author	Filip Kocica <xkocic01@stud.fit.vutbr.cz>
 * @date    05/10/2018
 *
 * Training dataset generator
 *
 * This file provides slecetion of ROIs
 */

#pragma once

// STD, STL
#include <iostream>
#include <vector>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/** @brief Position of ROI */
using coords     = std::pair<cv::Point, cv::Point>;

/** @brief Buffer of ROIs */
using ROI_buffer = std::vector<coords>;

/**
 * @brief Mouse click event handler
 * 
 * @param event [in] event type
 * @param x [in] vertical position
 * @param y [in] horizontal position
 * @param param [in] event param
 */
void mouseClick(int event, int x, int y, int flags, void *param);

/**
 * @brief Selection of ROIs by user
 * 
 * @param m [in, out] Input image
 * @return Buffer of ROIs
 */
ROI_buffer getRegionsOfInterest(cv::Mat& m);