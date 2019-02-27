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
DatasetGeneratorCropped_t::DatasetGeneratorCropped_t(std::ofstream& out, int imgClass, const StrBuffer_t& tsAnnots, const std::string& classID)
    : DatasetGenerator_t { out, imgClass }
	, m_tsAnnotations    { tsAnnots }
	, m_classID          { classID }
{
	m_distSignSize = PRNG::Uniform_t{40, 100};
}

///////////////////////////////////////////////////////////////////////////////////////////////
#ifdef ROI_SELECTION
void DatasetGeneratorCropped_t::generateDataset(const ROIBuffer_t& b, cv::Mat m, cv::Mat m2)
#else
void DatasetGeneratorCropped_t::generateDataset(cv::Mat m, cv::Mat m2)
#endif
{
	// Check whether image is not bigger than background
	while (m2.rows > m.rows || m2.cols > m.cols)
	{
		cv::resize(m2, m2, cv::Size{m2.cols / 10, m2.rows / 10});
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
	//double scale = ImageProcessing::resize(m2, x, m.cols/2, m_prngValue);
	double scale = 1.0;

	// Adjust position of sign
	cv::Mat a;
#ifdef PARTIAL_TS
	int prev_w = m2.cols;
	int prev_h = m2.rows;
#endif
	adjustPosition(m, m2, a, x, y);

	// Create annotation
	Annotation_t annotation;
	parseAnnotations(annotation);

	cv::Rect roi;
	roi.x      = scale * annotation.rx1;
	roi.y      = scale * annotation.ry1;
	roi.width  = scale * (annotation.rx2 - annotation.rx1);
	roi.height = scale * (annotation.ry2 - annotation.ry1);

	// Cropped because of partial TS
#ifdef PARTIAL_TS
	if ( prev_w != m2.cols )
	{
		roi.width = m2.cols - roi.x;
	}
	else if ( prev_h != m2.rows )
	{
		roi.height = m2.rows - roi.y;
	}
#endif

	cv::Mat tmp = m2(roi);

	int x2 = x + scale * annotation.rx1;
	int y2 = y + scale * annotation.ry1;

	createAnnotation(m, tmp, x2, y2);

	// Copy TS to background on specified position
	ImageProcessing::copy2bgCropped(m, m2, tmp, x, y, x2, y2);

	// DEBUG: Show bbox around TS
	showBbox(m, tmp, x2, y2);
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGeneratorCropped_t::parseAnnotations(Annotation_t& annot) const
{
	try
	{
		for (auto& it : m_tsAnnotations)
		{
			StrBuffer_t elems;

			splitAnnotation(it, ';', elems);

			if (m_classID == elems.at(AnnotationFormat_t::NAME))
			{
				annot = {elems.at(AnnotationFormat_t::NAME),
				         std::stoi(elems.at(AnnotationFormat_t::WIDTH)),
						 std::stoi(elems.at(AnnotationFormat_t::HEIGHT)),
						 std::stoi(elems.at(AnnotationFormat_t::RX1)),
						 std::stoi(elems.at(AnnotationFormat_t::RY1)),
						 std::stoi(elems.at(AnnotationFormat_t::RX2)),
						 std::stoi(elems.at(AnnotationFormat_t::RY2)),
						 elems.at(AnnotationFormat_t::CLASS)};
				return;
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Anntotaion has incorrect format. ("
					+ std::string(e.what()) + ")" << std::endl;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
void DatasetGeneratorCropped_t::splitAnnotation(const std::string& s, const char& delim, StrBuffer_t& elems) const
{
	std::stringstream ss(s);
	std::string item;

	while (std::getline(ss, item, delim))
	{
		elems.emplace_back(std::move(item));
	}
}