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
	0        1      2      3     4      5
	filename,x_from,y_from,width,height,sign_class
*/
enum AnnotationFormat
{
	NAME   = 0,
	X      = 1,
	Y      = 2,
	W      = 3,
	H      = 4,
	CLASS  = 5
};
    
int main(int argc, char **argv)
{
	// Bad args
	if (argc < 3)
	{
		std::cerr << "Usage: ./" << argv[0] << " annotation_csv path_2_images" << std::endl;
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
		// std::getline(infile, line);


		// Loop over records
		while (std::getline(infile, line))
		{
			// Parse comma-separated line
			std::vector<std::string> vec = split(line, ',');

			// Skip format info
			if ( vec.at(NAME) == "filename" )
			{
				continue;
			}

			// Create annotation file
			std::string annot = vec.at(NAME);
			annot = annot.substr(0, annot.find_last_of('.')) + ".txt";
			std::ofstream annotFile;
			annotFile.open("RTSD/detection/3/annotation/" + annot, std::ios_base::app);
			
			// Load annotated image
			m = cv::imread(path + vec.at(NAME));

			imgW = m.cols;
			imgH = m.rows;

			// Calculate YOLO relative annotations
			x     = std::stoi(vec.at(X));
			y     = std::stoi(vec.at(Y));
			signW = std::stoi(vec.at(W));
			signH = std::stoi(vec.at(H));

			annotFile << vec.at(CLASS)  << " ";
			annotFile << x     / imgW   << " ";
			annotFile << y     / imgH   << " ";
			annotFile << signW / imgW   << " ";
			annotFile << signH / imgH   << std::endl;


			// DEBUG
			/*cv::Point pt { (int) x,           (int) y };
			cv::Point pt2{ (int) (x + signW), (int) (y + signH) };

			cv::rectangle(m, pt, pt2, cv::Scalar{0, 255, 0});
			cv::imshow("1", m); cv::waitKey(0);

			std::cout << vec.at(CLASS)  << " ";
			std::cout << x     / imgW   << " ";
			std::cout << y     / imgH   << " ";
			std::cout << signW / imgW   << " ";
			std::cout << signH / imgH   << std::endl;*/
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Anntotaion has incorrect format. ("
					+ std::string(e.what()) + ")" << std::endl;
	}

	return 0;
}