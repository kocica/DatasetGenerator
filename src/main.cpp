/**
 * @file    main.cpp
 * @author  Filip Kocica <xkocic01@stud.fit.vutbr.cz>
 * @date    05/10/2018
 *
 * Training dataset generator
 *
 * Program entry point, first loads data & finds ROIs, then calls generating
 */

// Local
#include "ROI.hpp"
#include "Generator.hpp"
#include "Utils.hpp"

/**
 * @brief Entry point
 */
int main(int argc, char **argv)
{
	// Locals
	int               imgClass = 0, imgCounter = 0, ret;
	std::string       pathToBackgrounds, pathToImages, backgroundName, imageName, csvName;
	Utils::ImgBuffer  bgs, imgs;
	cv::Size          size;

	// Parse arguments
	if ((ret = Utils::parseArgs(argc, argv, pathToBackgrounds, pathToImages, size)) != 0)
	{
		return ret;
	}

	// Load iamges
	Utils::loadImages(pathToBackgrounds, bgs);
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
		std::ofstream annotFile(Utils::out + std::to_string(imgCounter) + Utils::annotExt);

		// Image & annotation generator
		DtstGenerator gen(annotFile, imgClass);
		gen.generate(roiBuffer, bg, img);

		// Show image
		//cv::imshow("bbox", bg);
		// Save img
		//cv::resize(bg, bg, cv::Size{416, 416});
		imwrite(Utils::out + std::to_string(imgCounter) + Utils::imageExt, bg);
	}

	// Free resources
	//cv::waitKey(0);
	cv::destroyAllWindows();
	return 0;
}