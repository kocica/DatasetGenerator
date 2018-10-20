/**
 * @file    ImageProcessing.cpp
 * @author  Filip Kocica <xkocic01@stud.fit.vutbr.cz>
 * @date    13/10/2018
 *
 * Training dataset generator
 *
 * This file provides image processing operations, such as 3D transformations
 */

#pragma once

// STD, STL
#include <iostream>
#include <stdlib.h>
#include <algorithm>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/**
 * @brief Namespace with image processing functions
 */
namespace ImageProcessing
{
	/**
	 * @brief 3D rotation function
	 * @param [in] input Input matrix
	 * @param [out] output Output matrix
	 * @param [in] alpha Angle X axis
	 * @param [in] beta Angle Y axis
	 * @param [in] gamma Angle Z axis
	 * @param [in] dx Translation along the X axis
	 * @param [in] dy Translation along the Y axis
	 * @param [in] dz Translation along the Z axis
	 * @param [in] f Focal distance (distance between camera and image, a smaller number exaggerates the effect)
	 * Credits to: Michael Jepson (http://jepsonsblog.blogspot.com/2012/11/rotation-in-3d-using-opencvs.html)
	 */
	void rotateImage(const cv::Mat &input, cv::Mat &output, double alpha, double beta, double gamma, double dx, double dy, double dz, double f);

	/**
	 * @brief Resize copied image due to its position in background
	 *
	 * @param [in] m image to be copied
	 * @param [in] pos position in background to image be copied
	 * @param [in] mid middle of background
	 */
	void resize(cv::Mat& m, int pos, int mid, int r);

	/**
	 * @brief Copy image to specified location in background
	 * 
	 * @param [in, out] bg Background where image is copied
	 * @param [in] img Image copied to background
	 * @param [in] alpha Alpha channel of image copied to background
	 * @param [in] x Position X
	 * @param [in] y Position Y
	 * @param [in] b Should be luminescence modified ? (RNG)
	 * @param [in] v Luminescence (RNG)
	 */
	void copy2bg(cv::Mat& bg, cv::Mat& img, cv::Mat& alpha, int& x, int& y, int& b, int& v);

	/**
	 * @brief Copy cropped image to specified location in background
	 * 
	 * @param [in, out] bg Background where image is copied
	 * @param [in] img Image copied to background
	 * @param [in] x Position X
	 * @param [in] y Position Y
	 * @param [in] b Should be luminescence modified ? (RNG)
	 * @param [in] v Luminescence (RNG)
	 */
	void copy2bgCropped(cv::Mat& bg, cv::Mat& img, int& x, int& y, int& b, int& v);

	/**
	 * @brief Rotate image in Y axis
	 * 
	 * @param [in, out] img Rotated image
	 * @param [in] angle Angle
	 */
	void rotateAngle(cv::Mat& img, double angle);
}