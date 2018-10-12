/**
 * @file    main.cpp
 * @author  Filip Kocica <xkocic01@stud.fit.vutbr.cz>
 * @date    05/10/2018
 *
 * Training dataset generator
 *
 * Program entry point, first loads data & finds ROIs, then calls generating
 */

#include "ROI.hpp"
#include "Generator.hpp"
//#include <filesystem>

/** @brief Buffer of images */
using ImgBuffer = std::vector<cv::Mat>;
/** @brief Buffer of strings */
using StrBuffer = std::vector<std::string>;

const std::string annotExt   = ".txt";
const std::string imageExt   = ".jpg";
const std::string out        = "out/";

void printUsage();
void loadImages(const std::string& path, ImgBuffer& imgs);
int parseArgs(int argc, char **argv, std::string& pathBgs, std::string& pathImgs, cv::Size& s);

/**
 * @brief Entry point
 */
int main(int argc, char **argv)
{
	// Locals
	int         imgClass = 0, imgCounter = 0, ret;
	std::string pathToBackgrounds, pathToImages, backgroundName, imageName, csvName;
	ImgBuffer   bgs, imgs;
	cv::Size    size;

	// Parse arguments
	if ((ret = parseArgs(argc, argv, pathToBackgrounds, pathToImages, size)) != 0)
	{
		return ret;
	}

	// Load iamges
	loadImages(pathToBackgrounds, bgs);
	cv::Mat img = cv::imread("data/stop.png", cv::IMREAD_UNCHANGED);

	// Load example background
	cv::Mat exampleBg = cv::imread("data/roi-selection.png");
	cv::resize(exampleBg, exampleBg, size);

	// Regions of interest selection
	ROI_buffer roiBuffer;
	roiBuffer = getRegionsOfInterest(exampleBg);

	int numberOfImages = bgs.size();
	cv::Mat bg;

	// For each image
	for (imgCounter = 0; imgCounter < numberOfImages; imgCounter++)
	{
		bg  = bgs.at(imgCounter);

		// Resize images
		cv::resize(bg, bg, size);

		// Output annotation file
		std::ofstream annotFile(out + std::to_string(imgCounter) + annotExt);

		// Image & annotation generator
		DtstGenerator gen(annotFile, imgClass);
		gen.generate(roiBuffer, bg, img);

		// Show image
		//cv::imshow("bbox", bg);
		// Save img
		//cv::resize(bg, bg, cv::Size{416, 416});
		imwrite(out + std::to_string(imgCounter) + imageExt, bg);
	}

	// Free resources
	//cv::waitKey(0);
	cv::destroyAllWindows();
	return 0;
}

void printUsage()
{
	std::cout << "Usage: dataset_generator" << std::endl;
}

int parseArgs(int argc, char **argv, std::string& pathBgs, std::string& pathImgs, cv::Size& s)
{
	if (argc < 5)
	{
		printUsage();
		return 1;
	}

	pathBgs  = argv[1];
	pathImgs = argv[2];
	s        = cv::Size{ std::stoi(argv[3]), std::stoi(argv[4]) };

	return 0;
}

void loadImages(const std::string& path, ImgBuffer& imgs)
{
	std::vector<cv::String> strBuffer;
	cv::glob(path, strBuffer, false);

	for (auto& it : strBuffer)
	{
		imgs.push_back(cv::imread(it));
	}
}

/*
StrBuffer getDirectories(const std::string& path)
{
    StrBuffer r;
    for(auto& p : std::filesystem::recursive_directory_iterator(path))
	{
        if(p.status().type() == std::filesystem::file_type::directory)
		{
            r.push_back(p.path().string());
		}
	}

    return r;
}
*/
