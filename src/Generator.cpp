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
DatasetGenerator_t::DatasetGenerator_t(std::ofstream& out, int imgClass)
	: m_out             { out }
	, m_class           { imgClass }
	, m_prngProbability { 0 }
	, m_prngValue       { 0 }
{
	srand(time(NULL));
	m_rng.seed(std::random_device()());

	m_probability = PRNG::Uniform_t{1, 100};
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGenerator_t::adjustPosition(const cv::Mat& m, cv::Mat& m2, cv::Mat& a, int& x, int& y)
{
	// If the sign is outside of background image bounds
	if ( ((x + m2.cols) >= m.cols) || ((y + m2.rows) >= m.rows) )
	{
#ifdef  PARTIAL_TS
		int overlap_X = (m.cols - x) > m2.cols ? m2.cols : m.cols - x;
		int overlap_Y = (m.rows - y) > m2.rows ? m2.rows : m.rows - y;

		if ( ( overlap_X < (m2.cols / 2) || overlap_Y < (m2.rows / 2) ) && m_probability( m_rng ) >= 10 ) // 90%
		{
#endif
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
#ifdef  PARTIAL_TS
		}
		else // 10%
		{
			// Crop just part of the sign
			m2 = m2( cv::Rect{ 0, 0, overlap_X, overlap_Y } );

			// In case of transparent TSs crop also alpha channel
			if ( ! a.empty() )
			{
				a = a( cv::Rect{ 0, 0, overlap_X, overlap_Y } );
			}
		}
#endif
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGenerator_t::createAnnotation(const cv::Mat& m, const cv::Mat& m2, const int& x, const int& y)
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
void DatasetGenerator_t::showBbox(cv::Mat& m, const cv::Mat& m2, const int& x, const int& y)
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