/**
 *
 */
 
#pragma once
 
// STD, STL
#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <fstream>
 
// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
 
class DtstGenerator
{
public:
	DtstGenerator(std::ofstream& out, int imgClass);
	virtual ~DtstGenerator();
 
	void generate(std::vector<std::pair<cv::Point, cv::Point>>& b, cv::Mat& m, cv::Mat& m2);
 
protected:
	void resize(cv::Mat& m, int pos, int mid);
	void createAnnotation(cv::Mat& m, cv::Mat& m2, int& x, int& y);
 
private:
	std::ofstream&  m_out;
	int             m_class;
};