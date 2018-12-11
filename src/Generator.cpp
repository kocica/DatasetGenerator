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

DtstGenerator::DtstGenerator(std::ofstream& out, int imgClass)
: m_out   { out }
, m_class { imgClass }
{
	srand(time(NULL));
	m_rng.seed(std::random_device()());

	dist2     = std::uniform_int_distribution<std::mt19937::result_type> (0, 1);
	dist10    = std::uniform_int_distribution<std::mt19937::result_type> (2, 10);
	dist15    = std::uniform_int_distribution<std::mt19937::result_type> (0, 10);
	dist20    = std::uniform_int_distribution<std::mt19937::result_type> (0, 20);
	dist30    = std::uniform_int_distribution<std::mt19937::result_type> (0, 30);
	dist50    = std::uniform_int_distribution<std::mt19937::result_type> (0, 50);

	distDiv   = std::uniform_int_distribution<std::mt19937::result_type> (10, 17);
	distAlpha = std::uniform_int_distribution<std::mt19937::result_type> (10, 17);
	distBeta  = std::uniform_int_distribution<std::mt19937::result_type> (0,  37);

	distGamma = std::uniform_int_distribution<std::mt19937::result_type> (110, 200);

#ifdef IMG_CROPPED
	dist100   = std::uniform_int_distribution<std::mt19937::result_type> (30, 100);
#else
	dist100   = std::uniform_int_distribution<std::mt19937::result_type> (20, 60);
#endif
}

DtstGenerator::~DtstGenerator()
{

}

void DtstGenerator::generate(std::vector<std::pair<cv::Point, cv::Point>>& b, cv::Mat m, cv::Mat m2)
{
	// RNG values
	int rng_dir,            // Should the image be rotated to "left" or "right"
	    rng_rot,            // Should image be rotated / blured / ...
	    rng_val;            // Pseudo-random generated value


	// Blur image with random kernel size
#ifdef BLUR
	rng_rot = dist20(m_rng);
	rng_val = dist20(m_rng);
	rng_val = (rng_val % 2) == 0 ? rng_val + 1: rng_val; // Generate even number (eg. 3, 5, 7, ...)

	if (rng_rot < 3)
	{
		cv::blur(m2, m2, cv::Size(rng_val, rng_val));
	}
#endif



	// Get alpha channel from transparent image
	std::vector<cv::Mat> channels;
	split(m2, channels);
	cv::Mat alpha;
	channels[3].copyTo(alpha);

	// Convert transparent image to BGR (3 channel) img
	cv::cvtColor(m2, m2, cv::COLOR_BGRA2BGR);


	// Resize inserted image to 50x50
	cv::resize(m2,    m2,    cv::Size(50, 50));
	cv::resize(alpha, alpha, cv::Size(50, 50));



	// Select random bounding box
#ifdef ROI_SELECTION
	std::uniform_int_distribution<std::mt19937::result_type> distN(0, b.size() - 1);
	int noBbox = distN(m_rng);
	auto bbox  = b.at(noBbox);

	int max_x = std::max(bbox.first.x, bbox.second.x);
	int min_x = std::min(bbox.first.x, bbox.second.x);
	int max_y = std::max(bbox.first.y, bbox.second.y);
	int min_y = std::min(bbox.first.y, bbox.second.y);
#else
	int max_x = m.cols;
	int min_x = 0;
	int max_y = m.rows;
	int min_y = 0;
#endif


	// Generate random position in bounding box
	std::uniform_int_distribution<std::mt19937::result_type> distN2(min_x, max_x);
	std::uniform_int_distribution<std::mt19937::result_type> distN3(min_y, max_y);

	int x = distN2(m_rng); 
	int y = distN3(m_rng);



	// Resize image due to its position in background
	rng_val = dist100(m_rng);
	ImageProcessing::resize(m2,     x, m.cols/2, rng_val);
	ImageProcessing::resize(alpha,  x, m.cols/2, rng_val);
	

	// Rotate image left/right
#ifdef ROTATE_XY
	rng_dir = dist2 (m_rng);
	rng_rot = dist20(m_rng);
	rng_val = dist10(m_rng);

	if (rng_rot < 6)
	{
		ImageProcessing::rotateAngle(m2,    rng_dir == 0 ? rng_val : -rng_val);
		ImageProcessing::rotateAngle(alpha, rng_dir == 0 ? rng_val : -rng_val);
	}
#endif


	// Rotate image Z
#ifdef ROTATE_Z
	rng_dir = dist2(m_rng);
	rng_rot = dist20(m_rng);
	rng_val = dist20(m_rng);

	if (rng_rot < 6)
	{
		rng_val = rng_dir == 0 ? rng_val : -rng_val;
		rng_val = 90 + rng_val;

		ImageProcessing::rotateImage(m2,    m2,    90, rng_val, 90, 0, 0, 200, 200);
		ImageProcessing::rotateImage(alpha, alpha, 90, rng_val, 90, 0, 0, 200, 200);
	}
#endif


	// Copy image to background
	if (((x + m2.cols) <= m.cols) && ((y + m2.rows) <= m.rows))
	{
		// OK
	}
	else
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

	// Create annotation
	createAnnotation(m, m2, x, y);

	// Hue
#ifdef HUE
	rng_dir = dist30( m_rng );

	if ( rng_dir < 15 )      // 50% Modify Hue
	{
		ImageProcessing::modifyHue(m2);
	}
#endif


	// Brightness & contrast adjustment
#ifdef BIGHTCONTRAST
	rng_dir = dist30( m_rng );

	if ( rng_dir < 10 )      // 33%
	{
		ImageProcessing::modifyLuminescence( m2, distAlpha( m_rng ) / 10., distBeta( m_rng ) );
	}
	else if ( rng_dir < 20 ) // 33%
	{
		m2.convertTo(m2, CV_32FC3);              // Convert to floating point, 3 channels
		cv::divide(m2, distDiv(m_rng) / 10, m2); // Divide (decrease brightness & contrast)
		m2.convertTo(m2, CV_8UC3);               // Convert back to unsigned integer, 3 channels
	}
	else                     // 34% Nothing 2 do
	{

	}
#endif

	// Gamma correction
#ifdef GAMMACORRECT
	ImageProcessing::gammaCorrection( m2, distGamma( m_rng ) / 100.0 );
#endif


	// Copy sign into the background on position (x, y)
	ImageProcessing::copy2bg(m, m2, alpha, x, y);


#ifdef GENERATOR_DEBUG
	// Show annotated bbox around sign
	showBbox(m, m2, x, y);
#endif
}

void DtstGenerator::generateCropped(std::vector<std::pair<cv::Point, cv::Point>>& b, cv::Mat m, cv::Mat m2)
{
	// RNG values
	int rng_dir,            // Should the image be rotated to "left" or "right"
	    rng_rot,            // Should image be rotated / blured
	    rng_val;            // Pseudo-random generated value

	// Check whether image is not bigger than background
	while (m2.rows > m.rows || m2.cols > m.cols)
	{
		cv::resize(m2, m2, cv::Size{m2.cols / 2, m2.rows / 2});
	}


	// Select random bounding box
#ifdef ROI_SELECTION
	std::uniform_int_distribution<std::mt19937::result_type> distN(0, b.size() - 1);
	int noBbox = distN(m_rng);
	auto bbox  = b.at(noBbox);

	int max_x = std::max(bbox.first.x, bbox.second.x);
	int min_x = std::min(bbox.first.x, bbox.second.x);
	int max_y = std::max(bbox.first.y, bbox.second.y);
	int min_y = std::min(bbox.first.y, bbox.second.y);
#else
	int max_x = m.cols;
	int min_x = 0;
	int max_y = m.rows;
	int min_y = 0;
#endif


	// Generate random position in bounding box
	std::uniform_int_distribution<std::mt19937::result_type> distN2(min_x, max_x);
	std::uniform_int_distribution<std::mt19937::result_type> distN3(min_y, max_y);

	int x = distN2(m_rng); 
	int y = distN3(m_rng);


	rng_val = dist100(m_rng);
	cv::resize(m2, m2, cv::Size{rng_val, rng_val});


	// Rotate image left/right
#ifdef ROTATE_XY
	rng_dir = dist2 (m_rng);
	rng_rot = dist20(m_rng);
	rng_val = dist10(m_rng);

	if (rng_rot < 6)
	{
		ImageProcessing::rotateAngle(m2,    rng_dir == 0 ? rng_val : -rng_val);
	}
#endif


	// Rotate image Z
#ifdef ROTATE_Z
	rng_dir = dist2(m_rng);
	rng_rot = dist20(m_rng);
	rng_val = dist20(m_rng);

	if (rng_rot < 6)
	{
		rng_val = rng_dir == 0 ? rng_val : -rng_val;
		rng_val = 90 + rng_val;

		ImageProcessing::rotateImage(m2,    m2,    90, rng_val, 90, 0, 0, 200, 200);
	}
#endif


	// Copy image to background
	if (((x + m2.cols) <= m.cols) && ((y + m2.rows) <= m.rows))
	{
		// OK
	}
	else
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

#ifdef GENERATOR_DEBUG
	showBbox(m, tmp, x2, y2);
#endif
}

void DtstGenerator::createAnnotation(cv::Mat& m, cv::Mat& m2, int& x, int& y)
{
	// Save image class
	m_out << m_class << " ";
	// Save relative position
	m_out << (x + ((double) m2.cols / 2)) / m.cols << " ";
	m_out << (y + ((double) m2.rows / 2)) / m.rows << " ";
	// Save relative proportion
	m_out << (double) m2.cols / m.cols << " ";
	m_out << (double) m2.rows / m.rows << std::endl;
}

void DtstGenerator::showBbox(cv::Mat& m, cv::Mat& m2, int& x, int& y)
{
	int x2     = ((x + ((double) m2.cols / 2)) / m.cols) * m.cols - ((double) m2.cols / 2);
	int y2     = ((y + ((double) m2.rows / 2)) / m.rows) * m.rows - ((double) m2.rows / 2);
	int size_x = ((double) m2.cols / m.cols) * m.cols;
	int size_y = ((double) m2.rows / m.rows) * m.rows;

	cv::Point pt {x2, y2};
	cv::Point pt2{x2 + size_x, y2 + size_y};

	cv::rectangle(m, pt, pt2, cv::Scalar{0, 255, 0}, 1);
}