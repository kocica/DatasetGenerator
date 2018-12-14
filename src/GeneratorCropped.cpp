/**
 * @file    GeneratorTransparent.cpp
 * @author  Filip Kocica <xkocic01@stud.fit.vutbr.cz>
 * @date    05/10/2018
 *
 * Training dataset generator
 *
 * This file provides generating of training images to backgrounds
 */

#include "GeneratorCropped.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////
DatasetGeneratorCropped_t::DatasetGeneratorCropped_t(std::ofstream& out, int imgClass)
    : DatasetGenerator_t{ out, imgClass }
{
	m_distSignSize = PRNG::Uniform_t{40, 100};
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGeneratorCropped_t::generateDataset(const std::vector<std::pair<cv::Point, cv::Point>>& b, cv::Mat m, cv::Mat m2)
{
	// Check whether image is not bigger than background
	while (m2.rows > m.rows || m2.cols > m.cols)
	{
		cv::resize(m2, m2, cv::Size{m2.cols / 2, m2.rows / 2});
	}


	// Select random bounding box
#	ifdef ROI_SELECTION
		PRNG::Uniform_t distN{0, b.size() - 1};
		int noBbox = distN(m_rng);
		auto bbox  = b.at(noBbox);

		size_t max_x = std::max(bbox.first.x, bbox.second.x);
		size_t min_x = std::min(bbox.first.x, bbox.second.x);
		size_t max_y = std::max(bbox.first.y, bbox.second.y);
		size_t min_y = std::min(bbox.first.y, bbox.second.y);
#	else
		size_t max_x = m.cols;
		size_t min_x = 0;
		size_t max_y = m.rows;
		size_t min_y = 0;
#	endif


	// Generate random position in bounding box
	PRNG::Uniform_t distN2{min_x, max_x};
	PRNG::Uniform_t distN3{min_y, max_y};

	int x = distN2(m_rng); 
	int y = distN3(m_rng);


	m_prngValue = m_distSignSize(m_rng);
	ImageProcessing::resize(m2,     x, m.cols/2, m_prngValue);


	// Adjust position of sign
	adjustPosition(m, m2, x, y);


	// Create annotation
	cv::Rect roi;
	roi.x      = m2.rows / 10;
	roi.y      = m2.cols / 10;
	roi.height = m2.rows - 2*(m2.rows / 10);
	roi.width  = m2.cols - 2*(m2.cols / 10);

	cv::Mat tmp = m2(roi);

	int x2 = x + m2.rows / 10;
	int y2 = y + m2.cols / 10;

	createAnnotation(m, tmp, x2, y2);

	ImageProcessing::copy2bgCropped(m, m2, x, y);

	showBbox(m, tmp, x2, y2);
}