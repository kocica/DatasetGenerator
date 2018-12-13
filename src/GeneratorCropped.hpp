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
 * @brief Class providing generating of images and its annotation using cropped TS's
 */
class DatasetGeneratorCropped_t : public DatasetGenerator_t
{

public:

    DatasetGeneratorCropped_t() = delete;
	DatasetGeneratorCropped_t(std::ofstream& out, int imgClass);
	virtual ~DatasetGeneratorCropped_t() = default;

	/************************************************************************************
	 * @brief Generating of images & annotation (Image of traffic sign with alpha channel)
	 *
	 * @param b [in] buffer of ROIs
	 * @param m [in] background
	 * @param m2 [in] image to be copied to background
	 */
	void generateDataset(const std::vector<std::pair<cv::Point, cv::Point>>& b, cv::Mat m, cv::Mat m2) override;

protected:

private:

};