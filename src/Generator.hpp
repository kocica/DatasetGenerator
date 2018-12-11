/**
 * @file    Generator.hpp
 * @author  Filip Kocica <xkocic01@stud.fit.vutbr.cz>
 * @date    05/10/2018
 *
 * Training dataset generator
 *
 * This file provides generating of training images to backgrounds
 */

#pragma once

// STD, STL
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <fstream>
#include <chrono>
#include <thread>
#include <random>

// Local
#include "ImageProcessing.hpp"

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/**
 * @brief Class providing generating of images and its annotation
 */
class DtstGenerator
{
public:
	DtstGenerator(std::ofstream& out, int imgClass);
	virtual ~DtstGenerator();

	/************************************************************************************
	 * @brief Generating of images & annotation (Image of traffic sign with alpha channel)
	 *
	 * @param b [in] buffer of ROIs
	 * @param m [in] background
	 * @param m2 [in] image to be copied to background
	 */
	void generate(const std::vector<std::pair<cv::Point, cv::Point>>& b, cv::Mat m, cv::Mat m2);

	/************************************************************************************
	 * @brief Generating of images & annotation (Image of TS from classification dataset)
	 *
	 * @param b [in] buffer of ROIs
	 * @param m [in] background
	 * @param m2 [in] image to be copied to background
	 */
	void generateCropped(const std::vector<std::pair<cv::Point, cv::Point>>& b, cv::Mat m, cv::Mat m2);

protected:

	/************************************************************************************
	 * @brief Blurs image according to uniform probability
	 *
	 * @param m [in, out] Blurred image
	 */
	void opBlur(cv::Mat& m);

	/************************************************************************************
	 * @brief Rotates image in Z axis according to uniform probability
	 *
	 * @param m [in, out] Rotated image
	 * @param alpha [in, out] Rotated alpha channel of image
	 */
	void opRotateZ(cv::Mat& m, cv::Mat& alpha);

	/************************************************************************************
	 * @brief Rotates image in Y axis according to uniform probability
	 *
	 * @param m [in, out] Rotated image
	 * @param alpha [in, out] Rotated alpha channel of image
	 */
	void opRotateY(cv::Mat& m, cv::Mat& alpha);

	/************************************************************************************
	 * @brief Modifies hue of image according to uniform probability
	 *
	 * @param m [in, out] Modified image
	 */
	void opAdjustHue(cv::Mat& m);

	/************************************************************************************
	 * @brief Modifies brightness of image according to uniform probability
	 *
	 * @param m [in, out] Modified image
	 */
	void opAdjustBrightness(cv::Mat& m);

	/************************************************************************************
	 * @brief Corrects gamma of image according to uniform probability
	 *
	 * @param m [in, out] Modified image
	 */
	void opGammaCorrection(cv::Mat& m);

	/************************************************************************************
	 * @brief If generated position is outside of bounds
	 *        method adjust position correctly
	 *
	 * @param m [in] m Background
	 * @param m2 [in] m2 Generated sign
	 * @param x [in, out] Position of generated sign in background
	 * @param y [in, out] Position of generated sign in background
	 */
	void adjustPosition(const cv::Mat& m, const cv::Mat& m2, int& x, int& y);

	/************************************************************************************
	 * @brief Create annotation file with same name as result image
	 *
	 * @param m [in] copied image
	 * @param m2 [in] background
	 * @param x [in] x position of image
	 * @param y [in] y position of image
	 */
	void createAnnotation(const cv::Mat& m, const cv::Mat& m2, const int& x, const int& y);

	/****** DEBUG ************************************************************************
	 * @brief Draws bbox (rectangle) around traffic sign according to annotation
	 *
	 * @param [in] m Background
	 * @param [in] m2 TS
	 * @param [in] x coord
	 * @param [in] y coord
	 */
	void showBbox(const cv::Mat& m, const cv::Mat& m2, const int& x, const int& y);

private:
	/** @brief Annotation output stream */
	std::ofstream& m_out;

	/** @brief Image class number */
	int m_class;

	/** @brief PRNG seed */
	std::mt19937 m_rng;
	/** @brief Probability 1-100 */
	int prngProbability;
	/** @brief PRNG value */
	int prngValue;

	/** @brief Pseudo-random uniform distribution for probability */
	PRNG::Uniform probability;
	/** @brief 1-5   Pseudo-random uniform distribution */
	PRNG::Uniform dist5;
	/** @brief 0-50  Pseudo-random uniform distribution */
	PRNG::Uniform dist50;
	/** @brief 40-100 / 20-60 Pseudo-random uniform distribution */
	PRNG::Uniform dist100;
	/** @brief Pseudo-random uniform distribution for brightness & contrast modification */
	PRNG::Uniform distAlpha;
	/** @brief Pseudo-random uniform distribution for brightness & contrast modification */
	PRNG::Uniform distBeta;
	/** @brief Pseudo-random uniform distribution for brightness & contrast modification */
	PRNG::Uniform distDiv;
	/** @brief Pseudo-random uniform distribution for gamma correction */
	PRNG::Uniform distGamma;
};