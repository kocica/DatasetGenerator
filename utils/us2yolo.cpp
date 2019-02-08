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
	Filename;
	Annotation tag;
	Upper left corner X;
	Upper left corner Y;
	Lower right corner X;
	Lower right corner Y;
	Occluded,On another road;
	Origin file;
	Origin frame number;
	Origin track;
	Origin track frame number
*/
enum AnnotationFormat
{
	NAME   = 0,
	CLASS  = 1,
	RX1    = 2,
	RY1    = 3,
	RX2    = 4,
	RY2    = 5,
};



int main(int argc, char **argv)
{
	// Bad args
	if (argc < 2)
	{
		std::cerr << "Usage <bin_name> <csv_annot>" << std::endl;
		return 1;
	}

	// Open csv with annotation
	std::ifstream infile(argv[1]);
	std::string   line;
	std::string   path = "./";
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
			std::string fn = annot.substr(annot.find_last_of("/\\") + 1);
			annot = fn.substr(0, fn.find_last_of('.')) + ".txt";
			std::ofstream annotFile("annotation/" + annot);
			
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
			annotFile << "0"            << " ";
			annotFile << x     / imgW   << " ";
			annotFile << y     / imgH   << " ";
			annotFile << signW / imgW   << " ";
			annotFile << signH / imgH   << std::endl;

			std::string cmd = "cp ";
			cmd += path + vec.at(NAME) + " ";
			cmd += "annotation/" + fn.substr(0, fn.find_last_of('.')) + ".png";
			system(cmd.c_str());


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