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
	m_dist50       = PRNG::Uniform_t{0, 40};
	m_distGradient = PRNG::Uniform_t{0, 180};
	m_distDiv      = PRNG::Uniform_t{10, 12};
	m_distAlpha    = PRNG::Uniform_t{10, 15};
	m_distBeta     = PRNG::Uniform_t{0, 30};
	m_distGamma    = PRNG::Uniform_t{115, 195};
	m_distSignSize = PRNG::Uniform_t{40, 170};
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

		if (m_prngProbability <= 20) // 20%
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

		if (m_prngProbability <= 20) // 20%
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
#ifdef REALISTIC_LIGHTNING
void DatasetGeneratorTransparent_t::opAdjustBrightness(cv::Mat& m, double factor)
#else
void DatasetGeneratorTransparent_t::opAdjustBrightness(cv::Mat& m)
#endif
{
#	ifdef BIGHTCONTRAST
		m_prngProbability = m_probability(m_rng);
		double alpha  = m_distAlpha( m_rng ) / 10.0;
		double beta   = m_distBeta( m_rng );
		double divide = m_distDiv(m_rng) / 10.0;

#       ifdef REALISTIC_LIGHTNING
			factor  = factor > 1.0 ? 1.0 : factor;
			factor  = factor < 0.2 ? 0.2 : factor;
			alpha  *= factor;
			beta   *= factor;
#       endif

		if ( m_prngProbability <= 80 ) // 80%
		{
			ImageProcessing::modifyLuminescence( m, alpha , beta );
		}
		/*else if ( m_prngProbability <= 20 ) // 20%
		{
			m.convertTo(m, CV_32FC3);                // Convert to floating point, 3 channels
			cv::divide(m, divide, m);                // Divide (decrease brightness & contrast)
			m.convertTo(m, CV_8UC3);                 // Convert back to unsigned integer, 3 channels
		}*/
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGeneratorTransparent_t::opGammaCorrection(cv::Mat& m)
{
#	ifdef GAMMACORRECT
		m_prngProbability = m_probability(m_rng);

		if ( m_prngProbability <= 25 ) // 25%
		{
			ImageProcessing::gammaCorrection( m, m_distGamma( m_rng ) / 100.0 );
		}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGeneratorTransparent_t::opGaussianNoise(cv::Mat& m)
{
#	ifdef GAUSSNOISE
		m_prngProbability = m_probability(m_rng);

		if ( m_prngProbability <= 5 ) // 5%
		{
			ImageProcessing::gaussianNoise( m );
		}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGeneratorTransparent_t::opSaltNPepperNoise(cv::Mat& m)
{
#	ifdef PEPPERNOISE
		m_prngProbability = m_probability(m_rng);

		if ( m_prngProbability <= 5 ) // 5%
		{
			ImageProcessing::saltNPepperNoise( m );
		}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGeneratorTransparent_t::opAddGradient(cv::Mat& m)
{
#	ifdef GRADIENT
		m_prngProbability = m_probability(m_rng);

		if ( m_prngProbability <= 25 ) // 25%
		{
			double alpha = .7;
			double beta  = 10;
			double deltaAlpha = 1.3 / m.rows;
			double deltaBeta  = 80  / m.rows;
			int dir = ( m_probability(m_rng) < 50 ) ? m_distGradient(m_rng) : - m_distGradient(m_rng);

			cv::Mat tmp = cv::Mat::zeros(cv::Size(2*m.cols, 2*m.rows), m.type());
			int x_coord = m.cols / 2;
			int y_coord = m.rows / 2;

			m.copyTo(tmp(cv::Rect(x_coord, y_coord, m.cols, m.rows)));
			
			ImageProcessing::rotateImage(tmp, tmp, 90, 90, 90 + dir, 0, 0, 200, 200);

			cv::Mat newImage;
			tmp.copyTo(newImage);

			// Do the operation for each pixel: newImage(i,j) = alpha*img(i,j) + beta
			for( int y = 0; y < tmp.rows; y++ )
			{
				for( int x = 0; x < tmp.cols; x++ )
				{
					for( int c = 0; c < 3; c++ )
					{
						newImage.at<cv::Vec3b>(y, x)[c] = cv::saturate_cast<uchar>( alpha * ( tmp.at<cv::Vec3b>(y, x)[c] ) + beta );
					}
				}

				if ( y > y_coord && y < (y_coord + m.cols) )
				{
					alpha += deltaAlpha;
					beta  += deltaBeta;
				}
			}

			ImageProcessing::rotateImage(newImage, newImage, 90, 90, 90 - dir, 0, 0, 200, 200);

			m = newImage(cv::Rect(x_coord, y_coord, m.cols, m.rows));
    	}
#	endif
}

///////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROI_SELECTION
void DatasetGeneratorTransparent_t::generateDataset(const ROIBuffer_t& b, cv::Mat m, cv::Mat m2)
#else
void DatasetGeneratorTransparent_t::generateDataset(cv::Mat m, cv::Mat m2)
#endif
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
	adjustPosition(m, m2, alpha, x, y);

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
#   ifdef REALISTIC_LIGHTNING
	opAdjustBrightness(m2, ImageProcessing::getImgBrightness( m(cv::Rect{x, y, m2.cols, m2.rows}) ));
#	else
	opAdjustBrightness(m2);
#	endif

	// Gamma correction
	opGammaCorrection(m2);

	// P-Random noise
	opGaussianNoise(m2);
	opSaltNPepperNoise(m2);

	// Random gradient
	opAddGradient(m2);

	/////////////////////// CREATE IMG & ANNOTATION ///////////////////////

	// Create annotation
	createAnnotation(m, m2, x, y);

	// Copy sign into the background on position (x, y)
	ImageProcessing::copy2bg(m, m2, alpha, x, y);

	// Show annotated bbox around sign
	showBbox(m, m2, x, y);
}