/**
 * @file    GeneratorTransparent.cpp
 * @author  Filip Kocica <xkocic01@stud.fit.vutbr.cz>
 * @date    05/10/2018
 *
 * Training dataset generator
 *
 * This file provides generating of training images to backgrounds
 */

#include "GeneratorTransparent.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////
DatasetGeneratorTransparent_t::DatasetGeneratorTransparent_t(std::ofstream& out, int imgClass)
    : DatasetGenerator_t{ out, imgClass }
{
	m_dist5        = PRNG::Uniform_t{1, 5};
	m_dist50       = PRNG::Uniform_t{0, 50};
	m_distDiv      = PRNG::Uniform_t{10, 13};
	m_distAlpha    = PRNG::Uniform_t{10, 10};
	m_distBeta     = PRNG::Uniform_t{0, 30};
	m_distGamma    = PRNG::Uniform_t{115, 195};
	m_distSignSize = PRNG::Uniform_t{40, 100};
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGeneratorTransparent_t::opBlur(cv::Mat& m)
{
#	ifdef BLUR
		m_prngProbability = m_probability(m_rng);
		m_prngValue       = m_dist5(m_rng);
		m_prngValue       = (m_prngValue % 2) == 0 ? m_prngValue + 1: m_prngValue; // Generate even number (eg. 3, 5, 7, ...)

		if (m_prngProbability <= 5)  // 5%
		{
			cv::blur(m, m, cv::Size(m_prngValue, m_prngValue));
		}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGeneratorTransparent_t::opRotateZ(cv::Mat& m, cv::Mat& alpha)
{
#	ifdef ROTATE_Z
		m_prngProbability = m_probability(m_rng);
		m_prngValue       = m_dist5(m_rng);

		if (m_prngProbability <= 30) // 30%
		{
			m_prngProbability = m_probability(m_rng);
			m_prngValue       = m_prngProbability <= 50 ? m_prngValue : -m_prngValue;

			ImageProcessing::rotateImage(m,     m,     90, 90, 90 + m_prngValue, 0, 0, 200, 200);
			ImageProcessing::rotateImage(alpha, alpha, 90, 90, 90 + m_prngValue, 0, 0, 200, 200);
		}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGeneratorTransparent_t::opRotateY(cv::Mat& m, cv::Mat& alpha)
{
#	ifdef ROTATE_Y
		m_prngProbability = m_probability(m_rng);
		m_prngValue       = m_dist50(m_rng);

		if (m_prngProbability <= 50) // 50%
		{
			m_prngProbability = m_probability(m_rng);
			m_prngValue       = m_prngProbability <= 50 ? m_prngValue : -m_prngValue;

			ImageProcessing::rotateImage(m,     m,     90, 90 + m_prngValue, 90, 0, 0, 200, 200);
			ImageProcessing::rotateImage(alpha, alpha, 90, 90 + m_prngValue, 90, 0, 0, 200, 200);
		}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGeneratorTransparent_t::opAdjustHue(cv::Mat& m)
{
#	ifdef HUE
		m_prngProbability = m_probability(m_rng);

		if ( m_prngProbability <= 50 )  // 50% Modify Hue
		{
			ImageProcessing::modifyHue(m);
		}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGeneratorTransparent_t::opAdjustBrightness(cv::Mat& m)
{
#	ifdef BIGHTCONTRAST
		m_prngProbability = m_probability(m_rng);

		if ( m_prngProbability <= 20 ) // 20%
		{
			ImageProcessing::modifyLuminescence( m, m_distAlpha( m_rng ) / 10., m_distBeta( m_rng ) );
		}
		else if ( m_prngProbability <= 40 ) // 20%
		{
			m.convertTo(m, CV_32FC3);                // Convert to floating point, 3 channels
			cv::divide(m, m_distDiv(m_rng) / 10, m); // Divide (decrease brightness & contrast)
			m.convertTo(m, CV_8UC3);                 // Convert back to unsigned integer, 3 channels
		}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGeneratorTransparent_t::opGammaCorrection(cv::Mat& m)
{
#	ifdef GAMMACORRECT
		m_prngProbability = m_probability(m_rng);

		if ( m_prngProbability <= 50 ) // 50%
		{
			ImageProcessing::gammaCorrection( m, m_distGamma( m_rng ) / 100.0 );
		}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGeneratorTransparent_t::generateDataset(const std::vector<std::pair<cv::Point, cv::Point>>& b, cv::Mat m, cv::Mat m2)
{
	std::vector<cv::Mat> channels;
	cv::Mat alpha;

	// Split 4 channels to 4 mats
	split(m2, channels);

	// Copy 4th channel (alpha) to alpha mat
	channels[3].copyTo(alpha);

	// Convert transparent image (4 channels) to BGR (3 channels) img
	cv::cvtColor(m2, m2, cv::COLOR_BGRA2BGR);

	// Select random bounding box or use whole image
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

	// Generate random position in bounding box or image
	PRNG::Uniform_t distN2{min_x, max_x};
	PRNG::Uniform_t distN3{min_y, max_y};

	int x = distN2(m_rng); 
	int y = distN3(m_rng);
	
	// Resize image due to its position in background or randomly
	m_prngValue = m_distSignSize(m_rng);
	ImageProcessing::resize(m2,     x, m.cols/2, m_prngValue);
	ImageProcessing::resize(alpha,  x, m.cols/2, m_prngValue);

	// Adjust position of sign
	adjustPosition(m, m2, x, y);

	/////////////////////// OPERATIONS ///////////////////////

	// Blur image with random kernel size
	opBlur(m2);

	// Rotate image Z
	opRotateZ(m2, alpha);

	// Rotate image Y
	opRotateY(m2, alpha);

	// Adjust image hue
	opAdjustHue(m2);

	// Brightness & contrast adjustment
	opAdjustBrightness(m2);

	// Gamma correction
	opGammaCorrection(m2);

	/////////////////////// CREATE IMG & ANNOTATION ///////////////////////

	// Create annotation
	createAnnotation(m, m2, x, y);

	// Copy sign into the background on position (x, y)
	ImageProcessing::copy2bg(m, m2, alpha, x, y);

	// Show annotated bbox around sign
	showBbox(m, m2, x, y);
}