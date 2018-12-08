// STD, STL
#include <iostream>
#include <vector>
#include <cstdlib>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

int main(int argc, char **argv)
{
	if (argc < 4)
	{
		std::cerr << "Usage: <bin_name> <path2images> <new_w> <new_h>" << std::endl;
		return 1;
	}

	std::vector<cv::String> strBuffer;

    cv::glob(cv::String{argv[1]} + cv::String{"*.jpg"}, strBuffer, false);

    for (auto& it : strBuffer)
    {
        cv::Mat img = cv::imread(it, cv::IMREAD_COLOR);

        cv::resize(img, img, cv::Size{atoi(argv[2]), atoi(argv[3])});

        cv::imwrite(it, img);
    }

    return 0;
}