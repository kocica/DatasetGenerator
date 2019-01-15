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
#include <random>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/photo.hpp>

/// @brief Pseudo-random number generator
namespace PRNG
{
	using Uniform_t = std::uniform_int_distribution<std::mt19937::result_type>;
}

/**
 * @brief Namespace with image processing functions
 */
namespace ImageProcessing
{
	/************************************************************************************
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

	/************************************************************************************
	 * @brief Resize copied image due to its position in background
	 *
	 * @param [in] m image to be copied
	 * @param [in] pos position in background to image be copied
	 * @param [in] mid middle of background
	 * @return Scale
	 */
	double resize(cv::Mat& m, int pos, int mid, int r);

	/************************************************************************************
	 * @brief Copy image to specified location in background
	 * 
	 * @param [in, out] bg Background where image is copied
	 * @param [in] img Image copied to background
	 * @param [in] alpha Alpha channel of image copied to background
	 * @param [in] x Position X
	 * @param [in] y Position Y
	 */
	void copy2bg(cv::Mat& bg, cv::Mat& img, cv::Mat& alpha, const int& x, const int& y);

	/************************************************************************************
	 * @brief Copy cropped image to specified location in background
	 * 
	 * @param [in, out] bg Background where image is copied
	 * @param [in] img Image copied to background
	 * @param [in] x Position X
	 * @param [in] y Position Y
	 */
	void copy2bgCropped(cv::Mat& bg, cv::Mat& img, cv::Mat& annot, const int& x, const int& y,
						const int& x2, const int& y2, const bool& useMask = false);

	/**** UNUSED ************************************************************************
	 * @brief Rotate image in Y axis
	 * 
	 * @param [in, out] img Rotated image
	 * @param [in] angle Angle
	 */
	void rotateAngle(cv::Mat& img, double angle)  __attribute__ ((unused));

	/************************************************************************************
	 * @brief Modify luminescence of image in order to formula (for each pixel of image):
	 *                                         $newImage(i,j) = alpha * image(i,j) + beta$
	 * 
	 * @param [in, out] img Image of which luminescence is modified
	 * @param [in] alpha alpha value [1.0-3.0]
	 * @param [in] beta beta value [0-100]
	 */
	void modifyLuminescence(cv::Mat& img, const double& alpha, const int& beta);

	/************************************************************************************
	 * @brief Non-linear operation used to encode and decode luminance
	 *
	 * @param [in, out] img Modified image
	 * @param [in] gamma Gamma value
	 */
	void gammaCorrection(cv::Mat& img, const double& gamma);

	/************************************************************************************
	 * @brief Slightly modify hue (color) of the sign
	 *
	 * @param [in, out] img Image of which hue is modified
	 */
	void modifyHue(cv::Mat& img);

	/************************************************************************************
	 * @brief Get average brightness of image
	 *
	 * @param [in] img Image of which brigthness we are looking for
	 * @return Value in floating point representing amount of brightness in image
	 */
	double getImgBrightness(const cv::Mat& img);

	/************************************************************************************
	 * @brief Additive Gaussian noise
	 *
	 * @param [in, out] img Image to be noised
	 */
	void gaussianNoise(cv::Mat& img);

	/************************************************************************************
	 * @brief Additive Salt and pepper noise
	 *
	 * @param [in, out] img Image to be noised
	 */
	void saltNPepperNoise(cv::Mat& img);
}