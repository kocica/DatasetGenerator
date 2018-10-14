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

	dist2   = std::uniform_int_distribution<std::mt19937::result_type> (0, 1);
	dist10  = std::uniform_int_distribution<std::mt19937::result_type> (3, 10);
	dist15  = std::uniform_int_distribution<std::mt19937::result_type> (0, 15);
	dist20  = std::uniform_int_distribution<std::mt19937::result_type> (0, 20);
	dist30  = std::uniform_int_distribution<std::mt19937::result_type> (0, 30);
	dist100 = std::uniform_int_distribution<std::mt19937::result_type> (0, 50);
}

DtstGenerator::~DtstGenerator()
{

}

void DtstGenerator::generate(std::vector<std::pair<cv::Point, cv::Point>>& b, cv::Mat m, cv::Mat m2)
{
	// RNG values
	int rng_dir,            // Should the image be rotated to "left" or "right"
	    rng_rot,            // Should image be rotated / blured
		rng_val,            // Pseudo-random generated value
		rows,
		cols;


	// Blur image with random kernel size
	rng_rot = dist2 (m_rng);
	rng_val = dist15(m_rng);
	rng_val = (rng_val % 2) == 0 ? rng_val + 1: rng_val;

	if (rng_rot)
	{
		cv::GaussianBlur(m2, m2, cv::Size(rng_val, rng_val), 0, 0);
	}



	// Get alpha channel from transparent image
	std::vector<cv::Mat> channels;
	split(m2, channels);
	cv::Mat alpha;
	channels[3].copyTo(alpha);



	// Convert transparent image to BGR (3 channel) img
	cv::cvtColor(m2, m2, cv::COLOR_BGRA2BGR);



	// Resize inserted image to 100x100
	rows = m.rows/3;
	cols = m.cols/3;
	cv::resize(m2,    m2,    cv::Size(cols, rows));
	cv::resize(alpha, alpha, cv::Size(cols, rows));



	// Select random bounding box
	std::uniform_int_distribution<std::mt19937::result_type> distN(0, b.size() - 1);
	int noBbox = distN(m_rng);
	auto bbox  = b.at(noBbox);

	int max_x = std::max(bbox.first.x, bbox.second.x);
	int min_x = std::min(bbox.first.x, bbox.second.x);
	int max_y = std::max(bbox.first.y, bbox.second.y);
	int min_y = std::min(bbox.first.y, bbox.second.y);



	// Generate random position in bounding box
	std::uniform_int_distribution<std::mt19937::result_type> distN2(min_x, max_x);
	std::uniform_int_distribution<std::mt19937::result_type> distN3(min_y, max_y);

	int x = distN2(m_rng); 
	int y = distN3(m_rng);



	// Resize image due to its position in background
	rng_val = dist10(m_rng);
	ImageProcessing::resize(m2,     x, m.cols/2, rng_val);
	ImageProcessing::resize(alpha,  x, m.cols/2, rng_val);




	rows = m2.rows;
	cols = m2.cols;

	


	// Rotate image left/right
	rng_dir = dist2 (m_rng);
	rng_rot = dist2 (m_rng);
	rng_val = dist20(m_rng);     // Value [0; 20]

	if (rng_rot)
	{
		ImageProcessing::rotateAngle(m2,    rng_dir == 0 ? rng_val : -rng_val);
		ImageProcessing::rotateAngle(alpha, rng_dir == 0 ? rng_val : -rng_val);
	}



	// Rotate image Z
	rng_dir = dist2 (m_rng);
	rng_rot = dist2 (m_rng);
	rng_val = dist30(m_rng);     // Value [0; 30]

	if (rng_rot)
	{
		rng_val = rng_dir == 0 ? rng_val : -rng_val;
		rng_val = 90 + rng_val;

		ImageProcessing::rotateImage(m2,    m2,    90, rng_val, 90, 0, 0, 200, 200);
		ImageProcessing::rotateImage(alpha, alpha, 90, rng_val, 90, 0, 0, 200, 200);
	}



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



	// Save data to annotation file
	cv::Rect roi;
	roi.x      = (m2.cols - cols) / 2;
	roi.y      = (m2.rows - rows) / 2;
	roi.height = rows;
	roi.width  = cols;

	m2    = m2(roi);
	alpha = alpha(roi);

	createAnnotation(m, m2, x, y);



	// Increase/decrease luminescence and copy img to background
	rng_rot = dist2  (m_rng);
	rng_val = dist100(m_rng);     // Value [0; 100]

	ImageProcessing::copy2bg(m, m2, alpha, x, y, rng_rot, rng_val);
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


	// DEBUG, TODO: Remove
	/*int x2     = ((x + ((double) m2.cols / 2)) / m.cols) * m.cols - ((double) m2.cols / 2);
	int y2     = ((y + ((double) m2.rows / 2)) / m.rows) * m.rows - ((double) m2.rows / 2);
	int size_x = ((double) m2.cols / m.cols) * m.cols;
	int size_y = ((double) m2.rows / m.rows) * m.rows;

	cv::Point pt {x2, y2};
	cv::Point pt2{x2 + size_x, y2 + size_y};

	cv::rectangle(m, pt, pt2, cv::Scalar{0, 255, 0});*/
}