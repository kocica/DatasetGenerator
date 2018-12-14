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

/*
	CSV format:
	-----------

	Filename        - Image file the following information applies to
	Roi.x1,Roi.y1,
	Roi.x2,Roi.y2   - Location of the sign within the image
			  		  (Images contain a border around the actual sign
	                  of 10 percent of the sign size, at least 5 pixel)
	ClassId         - The class of the traffic sign
*/
enum AnnotationFormat
{
	NAME   = 0,
	RX1    = 1,
	RY1    = 2,
	RX2    = 3,
	RY2    = 4,
	CLASS  = 5
};
    
int main(int argc, char **argv)
{
	// Bad args
	if (argc < 3)
	{
		std::cerr << "TODO: Usage" << std::endl;
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
		//std::getline(infile, line);

		// Loop over records
		while (std::getline(infile, line))
		{
			// Parse comma-separated line
			std::vector<std::string> vec = split(line, ';');

			// Create annotation file
			std::string annot = vec.at(NAME);
			annot = annot.substr(0, annot.find_last_of('.')) + ".txt";
			std::ofstream annotFile("annotation/" + annot);
			
			// Load annotated image
			m = cv::imread(path + vec.at(NAME), cv::IMREAD_UNCHANGED);

			// Calculate YOLO relative annotations
			x     = (std::stoi(vec.at(RX1)) + std::stoi(vec.at(RX2))) / 2.0;
			y     = (std::stoi(vec.at(RY1)) + std::stoi(vec.at(RY2))) / 2.0;
			imgW  = std::stoi(vec.at(WIDTH));
			imgH  = std::stoi(vec.at(HEIGHT));
			signW = std::stoi(vec.at(RX2)) - std::stoi(vec.at(RX1));
			signH = std::stoi(vec.at(RY2)) - std::stoi(vec.at(RY1));

			//annotFile << vec.at(CLASS)  << " ";
			annotFile << "0"            << " ";
			annotFile << x     / imgW   << " ";
			annotFile << y     / imgH   << " ";
			annotFile << signW / imgW   << " ";
			annotFile << signH / imgH   << std::endl;


			/*int x2 = x - (signW / 2);
			int y2 = y - (signH / 2);
			int size_x = signW;
			int size_y = signH;
			cv::Point pt {x2, y2};
			cv::Point pt2{x2 + size_x, y2 + size_y};

			cv::rectangle(m, pt, pt2, cv::Scalar{0, 255, 0});
			cv::imshow("1", m); cv::waitKey(0);*/
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Anntotaion has incorrect format. ("
					+ std::string(e.what()) + ")" << std::endl;
	}

	return 0;
}