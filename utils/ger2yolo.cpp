// STD; STL
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


void loadImages(const std::string& path, std::vector<cv::Mat>& imgs, const int& mode)
{
    std::vector<cv::String> strBuffer;
    cv::glob(path, strBuffer, false);

    for (auto& it : strBuffer)
    {
        imgs.push_back(cv::imread(it, mode));
    }
}

std::vector<std::string> split(const std::string &s, char delim)
{
	std::stringstream ss(s);
	std::string item;
	std::vector<std::string> elems;

	while (std::getline(ss, item, delim))
	{
		elems.push_back(std::move(item));
	}

	return elems;
}

auto getImgClass = [&](const std::string& path) -> int
{
    std::fstream fs { path, std::ios_base::in };
    int ret; fs >> ret; return(ret);
};

/*
	CSV format:
	-----------
    0        1     2      3      4      5      6      7
	Filename;Width;Height;Roi.X1;Roi.Y1;Roi.X2;Roi.Y2;ClassId
*/
enum AnnotationFormat
{
	NAME   = 0,
	WIDTH  = 1,
	HEIGHT = 2,
	RX1    = 3,
	RY1    = 4,
	RX2    = 5,
	RY2    = 6,
	CLASS  = 7
};

// Compile: g++ -std=c++17 utils/ger2yolo.cpp -o ~/Desktop/ger2yolo `pkg-config --libs opencv`
// Run    : ~/Desktop/ger2yolo data/cropped/noentry/annotation.csv data/cropped/noentry/

int main(int argc, char **argv)
{
	// Bad args
	if (argc < 3)
	{
		std::cerr << "<bin_name> <path_2_csv> <path_2_save_annotations>" << std::endl;
		return 1;
	}

	// Open csv with annotation
	std::ifstream infile(argv[1]);
	std::string   line;
	std::string   path(argv[2]);
	cv::Mat       m;
	double 		  x, y, imgW, imgH, signW, signH;

	try
	{
	    // Skip header
		std::getline(infile, line);

		// Loop over records
		while (std::getline(infile, line))
		{
			// Parse comma-separated line
			std::vector<std::string> vec = split(line, ';');

			// Create annotation file
			std::string annot = vec.at(NAME);
			annot = annot.substr(0, annot.find_last_of('.')) + ".txt";

			// TODO: If u want to append annotation, uncomment app on following line
			std::ofstream annotFile(path + annot/*, std::ios_base::app*/);

			// Load annotated image
			m = cv::imread(path + vec.at(NAME), cv::IMREAD_UNCHANGED);

			// Calculate YOLO relative annotations
			x     = (std::stoi(vec.at(RX1)) + std::stoi(vec.at(RX2))) / 2.0;
			y     = (std::stoi(vec.at(RY1)) + std::stoi(vec.at(RY2))) / 2.0;
			signW = std::stoi(vec.at(RX2)) - std::stoi(vec.at(RX1));
			signH = std::stoi(vec.at(RY2)) - std::stoi(vec.at(RY1));
			imgW  = m.cols;
			imgH  = m.rows;

			//annotFile << vec.at(CLASS)  << " ";
			annotFile << getImgClass(path + "/imgClass") << " ";
			annotFile << x     / imgW            << " ";
			annotFile << y     / imgH            << " ";
			annotFile << signW / imgW            << " ";
			annotFile << signH / imgH            << std::endl;


			// DEBUG
			// Show bboxes
			int x2 = x - (signW / 2);
			int y2 = y - (signH / 2);
			int size_x = signW;
			int size_y = signH;
			cv::Point pt {x2, y2};
			cv::Point pt2{x2 + size_x, y2 + size_y};

			std::cout << path + vec.at(NAME) << std::endl;

			cv::rectangle(m, pt, pt2, cv::Scalar{0, 255, 0});
			cv::imshow("1", m); cv::waitKey(0);
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Anntotaion has incorrect format. ("
					+ std::string(e.what()) + ")" << std::endl;
	}

	return 0;
}
