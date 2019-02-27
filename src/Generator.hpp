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
#include <utility>

// Local
#include "ImageProcessing.hpp"
#ifdef ROI_SELECTION
#include "ROI.hpp"
#endif

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/**
 * @brief Base class providing generating of images and its annotation
 */
class DatasetGenerator_t
{

public:

	explicit DatasetGenerator_t(std::ofstream& out, int imgClass);
	virtual ~DatasetGenerator_t() = default;

	/************************************************************************************
	 * @brief Generating of images & annotation (Image of traffic sign with alpha channel)
	 *
	 * @param b [in] buffer of ROIs
	 * @param m [in] background
	 * @param m2 [in] image to be copied to background
	 */
#ifdef ROI_SELECTION
	virtual void generateDataset(const ROIBuffer_t& b, cv::Mat m, cv::Mat m2) = 0;
#else
	virtual void generateDataset(cv::Mat m, cv::Mat m2) = 0;
#endif

protected:

	/************************************************************************************
	 * @brief If generated position is outside of bounds
	 *        method adjust position correctly
	 *
	 * @param m [in] m Background
	 * @param m2 [in] m2 Generated sign
	 * @param a [in] TS alpha channel matrix
	 * @param x [in, out] Position of generated sign in background
	 * @param y [in, out] Position of generated sign in background
	 */
	void adjustPosition(const cv::Mat& m, cv::Mat& m2, cv::Mat& a, int& x, int& y);

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
	void showBbox(cv::Mat& m, const cv::Mat& m2, const int& x, const int& y);


	/** @brief Probability 1-100 */
	int m_prngProbability;
	/** @brief PRNG value */
	int m_prngValue;

	/** @brief PRNG seed */
	std::mt19937 m_rng;
	/** @brief Pseudo-random uniform distribution for probability */
	PRNG::Uniform_t m_probability;
	/** @brief TS size pseudo-random uniform distribution */
	PRNG::Uniform_t m_distSignSize;

private:

	/** @brief Annotation output stream */
	std::ofstream& m_out;

	/** @brief Image class number */
	int m_class;
};