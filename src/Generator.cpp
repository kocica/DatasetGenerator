/**
 * @file    Generator.cpp
 * @author  Filip Kocica <xkocic01@stud.fit.vutbr.cz>
 * @date    05/10/2018
 *
 * Training dataset generator
 *
 * This file provides generating of training images to backgrounds
 */

#include "Generator.hpp"

///////////////////////////////////////////////////////////////////////////////////////////////
DtstGenerator::DtstGenerator(std::ofstream& out, int imgClass)
: m_out   { out }
, m_class { imgClass }
{
	srand(time(NULL));
	m_rng.seed(std::random_device()());

	probability = PRNG::Uniform(1, 100);
	dist5       = PRNG::Uniform(1, 5);
	dist50      = PRNG::Uniform(0, 50);
#	ifdef IMG_CROPPED
	dist100     = PRNG::Uniform(40, 100);
#	else
	dist100     = PRNG::Uniform(20, 60);
#	endif
	distDiv     = PRNG::Uniform(10, 13);
	distAlpha   = PRNG::Uniform(10, 10);
	distBeta    = PRNG::Uniform(0,  30);
	distGamma   = PRNG::Uniform(115, 195);
}

///////////////////////////////////////////////////////////////////////////////////////////////
DtstGenerator::~DtstGenerator()
{

}

///////////////////////////////////////////////////////////////////////////////////////////////
void DtstGenerator::opBlur(cv::Mat& m)
{
#	ifdef BLUR
		prngProbability = probability(m_rng);
		prngValue       = dist5(m_rng);
		prngValue       = (prngValue % 2) == 0 ? prngValue + 1: prngValue; // Generate even number (eg. 3, 5, 7, ...)

		if (prngProbability <= 5)  // 5%
		{
			cv::blur(m, m, cv::Size(prngValue, prngValue));
		}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DtstGenerator::opRotateZ(cv::Mat& m, cv::Mat& alpha)
{
#	ifdef ROTATE_Z
		prngProbability = probability(m_rng);
		prngValue       = dist5(m_rng);

		if (prngProbability <= 30) // 30%
		{
			prngProbability = probability(m_rng);
			prngValue       = prngProbability <= 50 ? prngValue : -prngValue;

			ImageProcessing::rotateImage(m,     m,     90, 90, 90 + prngValue, 0, 0, 200, 200);
			ImageProcessing::rotateImage(alpha, alpha, 90, 90, 90 + prngValue, 0, 0, 200, 200);
		}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DtstGenerator::opRotateY(cv::Mat& m, cv::Mat& alpha)
{
#	ifdef ROTATE_Y
		prngProbability = probability(m_rng);
		prngValue       = dist50(m_rng);

		if (prngProbability <= 50) // 50%
		{
			prngProbability = probability(m_rng);
			prngValue       = prngProbability <= 50 ? prngValue : -prngValue;

			ImageProcessing::rotateImage(m,     m,     90, 90 + prngValue, 90, 0, 0, 200, 200);
			ImageProcessing::rotateImage(alpha, alpha, 90, 90 + prngValue, 90, 0, 0, 200, 200);
		}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DtstGenerator::opAdjustHue(cv::Mat& m)
{
#	ifdef HUE
		prngProbability = probability(m_rng);

		if ( prngProbability <= 50 )  // 50% Modify Hue
		{
			ImageProcessing::modifyHue(m);
		}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DtstGenerator::opAdjustBrightness(cv::Mat& m)
{
#	ifdef BIGHTCONTRAST
		prngProbability = probability(m_rng);

		if ( prngProbability <= 20 ) // 20%
		{
			ImageProcessing::modifyLuminescence( m, distAlpha( m_rng ) / 10., distBeta( m_rng ) );
		}
		else if ( prngProbability <= 40 ) // 20%
		{
			m.convertTo(m, CV_32FC3);              // Convert to floating point, 3 channels
			cv::divide(m, distDiv(m_rng) / 10, m); // Divide (decrease brightness & contrast)
			m.convertTo(m, CV_8UC3);               // Convert back to unsigned integer, 3 channels
		}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DtstGenerator::opGammaCorrection(cv::Mat& m)
{
#	ifdef GAMMACORRECT
		prngProbability = probability(m_rng);

		if ( prngProbability <= 50 ) // 50%
		{
			ImageProcessing::gammaCorrection( m, distGamma( m_rng ) / 100.0 );
		}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DtstGenerator::adjustPosition(const cv::Mat& m, const cv::Mat& m2, int& x, int& y)
{
	// If the sign is outside of bounds
	if ( ! (((x + m2.cols) <= m.cols) && ((y + m2.rows) <= m.rows)) )
	{
		// Shift image left
		if ((x + m2.cols) > m.cols)
		{
			x -= (x + m2.cols) - m.cols;
		}
		// Shift image up
		if ((y + m2.rows) > m.rows)
		{
			y -= (y + m2.rows) - m.rows;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DtstGenerator::createAnnotation(const cv::Mat& m, const cv::Mat& m2, const int& x, const int& y)
{
#	ifdef ANNOTATION
		// Save image class
		m_out << m_class << " ";
		// Save relative position
		m_out << (x + ((double) m2.cols / 2)) / m.cols << " ";
		m_out << (y + ((double) m2.rows / 2)) / m.rows << " ";
		// Save relative proportion
		m_out << (double) m2.cols / m.cols << " ";
		m_out << (double) m2.rows / m.rows << std::endl;
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DtstGenerator::showBbox(const cv::Mat& m, const cv::Mat& m2, const int& x, const int& y)
{
#	ifdef GENERATOR_DEBUG
		int x2     = ((x + ((double) m2.cols / 2)) / m.cols) * m.cols - ((double) m2.cols / 2);
		int y2     = ((y + ((double) m2.rows / 2)) / m.rows) * m.rows - ((double) m2.rows / 2);
		int size_x = ((double) m2.cols / m.cols) * m.cols;
		int size_y = ((double) m2.rows / m.rows) * m.rows;

		cv::Point pt {x2, y2};
		cv::Point pt2{x2 + size_x, y2 + size_y};

		cv::rectangle(m, pt, pt2, cv::Scalar{0, 255, 0}, 1);
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DtstGenerator::generate(const std::vector<std::pair<cv::Point, cv::Point>>& b, cv::Mat m, cv::Mat m2)
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
		PRNG::Uniform distN(0, b.size() - 1);
		int noBbox = distN(m_rng);
		auto bbox  = b.at(noBbox);

		int max_x = std::max(bbox.first.x, bbox.second.x);
		int min_x = std::min(bbox.first.x, bbox.second.x);
		int max_y = std::max(bbox.first.y, bbox.second.y);
		int min_y = std::min(bbox.first.y, bbox.second.y);
#	else
		int max_x = m.cols;
		int min_x = 0;
		int max_y = m.rows;
		int min_y = 0;
#	endif

	// Generate random position in bounding box or image
	PRNG::Uniform distN2(min_x, max_x);
	PRNG::Uniform distN3(min_y, max_y);

	int x = distN2(m_rng); 
	int y = distN3(m_rng);
	
	// Resize image due to its position in background or randomly
	prngValue = dist100(m_rng);
	ImageProcessing::resize(m2,     x, m.cols/2, prngValue);
	ImageProcessing::resize(alpha,  x, m.cols/2, prngValue);

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

///////////////////////////////////////////////////////////////////////////////////////////////
void DtstGenerator::generateCropped(const std::vector<std::pair<cv::Point, cv::Point>>& b, cv::Mat m, cv::Mat m2)
{
	// RNG values
	int prngProbability;
	int prngValue;


	// Check whether image is not bigger than background
	while (m2.rows > m.rows || m2.cols > m.cols)
	{
		cv::resize(m2, m2, cv::Size{m2.cols / 2, m2.rows / 2});
	}


	// Select random bounding box
#	ifdef ROI_SELECTION
		PRNG::Uniform distN(0, b.size() - 1);
		int noBbox = distN(m_rng);
		auto bbox  = b.at(noBbox);

		int max_x = std::max(bbox.first.x, bbox.second.x);
		int min_x = std::min(bbox.first.x, bbox.second.x);
		int max_y = std::max(bbox.first.y, bbox.second.y);
		int min_y = std::min(bbox.first.y, bbox.second.y);
#	else
		int max_x = m.cols;
		int min_x = 0;
		int max_y = m.rows;
		int min_y = 0;
#	endif


	// Generate random position in bounding box
	PRNG::Uniform distN2(min_x, max_x);
	PRNG::Uniform distN3(min_y, max_y);

	int x = distN2(m_rng); 
	int y = distN3(m_rng);


	prngValue = dist100(m_rng);
	ImageProcessing::resize(m2,     x, m.cols/2, prngValue);


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