/**
 * @file    GeneratorTransparent.hpp
 * @author  Filip Kocica <xkocic01@stud.fit.vutbr.cz>
 * @date    05/10/2018
 *
 * Training dataset generator
 *
 * This file provides generating of training images to backgrounds
 */

#pragma once

// Local
#include "Generator.hpp"

/**
 * @brief Class providing generating of images and its annotation using transparent TS's
 */
class DatasetGeneratorTransparent_t : public DatasetGenerator_t
{

public:

    DatasetGeneratorTransparent_t() = delete;
	explicit DatasetGeneratorTransparent_t(std::ofstream& out, int imgClass);
	virtual ~DatasetGeneratorTransparent_t() = default;

	/************************************************************************************
	 * @brief Generating of images & annotation (Image of traffic sign with alpha channel)
	 *
	 * @param b [in] buffer of ROIs
	 * @param m [in] background
	 * @param m2 [in] image to be copied to background
	 */
	void generateDataset(const std::vector<std::pair<cv::Point, cv::Point>>& b, cv::Mat m, cv::Mat m2) override;

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

    void opGaussianNoise(cv::Mat& m);
    void opSaltNPepperNoise(cv::Mat& m);

private:

	/** @brief 1-5   Pseudo-random uniform distribution */
	PRNG::Uniform_t m_dist5;
	/** @brief 0-50  Pseudo-random uniform distribution */
	PRNG::Uniform_t m_dist50;
	/** @brief Pseudo-random uniform distribution for brightness & contrast modification */
	PRNG::Uniform_t m_distAlpha;
	/** @brief Pseudo-random uniform distribution for brightness & contrast modification */
	PRNG::Uniform_t m_distBeta;
	/** @brief Pseudo-random uniform distribution for brightness & contrast modification */
	PRNG::Uniform_t m_distDiv;
	/** @brief Pseudo-random uniform distribution for gamma correction */
	PRNG::Uniform_t m_distGamma;
};