/**
 * @file    ROI.hpp
 * @author  Filip Kocica <xkocic01@stud.fit.vutbr.cz>
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
using Coords_t    = std::pair<cv::Point, cv::Point>;

/** @brief Buffer of ROIs */
using ROIBuffer_t = std::vector<Coords_t>;

/************************************************************************************
 * @brief Mouse click event handler
 * 
 * @param [in] event event type
 * @param [in] x vertical position
 * @param [in] y horizontal position
 * @param [in] param event param
 */
void mouseClick(int event, int x, int y, int flags, void *param);

/************************************************************************************
 * @brief Selection of ROIs by user
 * 
 * @param [in, out] m Input image
 * @return Buffer of ROIs
 */
ROIBuffer_t getRegionsOfInterest(cv::Mat& m);