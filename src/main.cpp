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
	int               imgClass = 1, imgCounter = 0, ret;
	std::string       pathToBackgrounds, pathToImages, backgroundName, imageName, csvName, classID = "stop";
	Utils::ImgBuffer  bgs, imgs;
	cv::Size          size;

	// Parse arguments
	if ((ret = Utils::parseArgs(argc, argv, pathToBackgrounds, pathToImages, size)) != 0)
	{
		return ret;
	}

	// Load iamges
	Utils::loadImages(pathToBackgrounds, bgs,  cv::IMREAD_COLOR);
	Utils::loadImages(pathToImages,      imgs, cv::IMREAD_UNCHANGED);
	//Utils::loadImages(pathToImages,      imgs, cv::IMREAD_COLOR);

	// Load example background
	//cv::Mat exampleBg = cv::imread("data/roi-selection.png");
	//cv::resize(exampleBg, exampleBg, cv::Size{704, 480} /*size*/);

	// Regions of interest selection
	ROI_buffer roiBuffer;
	//roiBuffer = getRegionsOfInterest(exampleBg);

	int numberOfImages = bgs.size();
	cv::Mat bg;

	// RNG
	std::mt19937 rng;
	rng.seed(std::random_device()());
	std::uniform_int_distribution<std::mt19937::result_type> distX;
	std::uniform_int_distribution<std::mt19937::result_type> distY;
	//std::uniform_int_distribution<std::mt19937::result_type> distW;
	//std::uniform_int_distribution<std::mt19937::result_type> distH;
	//std::uniform_int_distribution<std::mt19937::result_type> distB;
	std::uniform_int_distribution<std::mt19937::result_type> distI;
	distX = std::uniform_int_distribution<std::mt19937::result_type> (0, bgs.at(0).cols - 450);
	distY = std::uniform_int_distribution<std::mt19937::result_type> (0, bgs.at(0).rows - 350);
	//distW = std::uniform_int_distribution<std::mt19937::result_type> (225, 300);
	//distH = std::uniform_int_distribution<std::mt19937::result_type> (150, 200);
	//distB = std::uniform_int_distribution<std::mt19937::result_type> (0, 20);
	distI = std::uniform_int_distribution<std::mt19937::result_type> (0, imgs.size() - 1);

	// For each image
	size_t i = 0;
	bool blur = false;

	for (imgCounter = 0; imgCounter < 200; imgCounter++)
	{
		bgs.at(imgCounter % numberOfImages).copyTo(bg);

		//int w = distW(rng), h = distH(rng);
		/*if (distB(rng) < 3)
		{
			std::swap(w, h);
		}*/

		int w = 407, h = 309;

		cv::Rect roi {(int) distX(rng), (int) distY(rng), w, h};
		bg = bg(roi);

		// Output annotation file
		std::ofstream annotFile(Utils::out + classID + std::to_string(imgCounter) + Utils::annotExt);

		// Image & annotation generator
		DtstGenerator gen(annotFile, imgClass);

		blur = gen.generate(roiBuffer, bg, imgs.at(i /*distI(rng)*/));

		// Blur image with kernel of size (3;3) with probability ~5%
		/*if ((distB(rng) < 2) && blur)
		{
			cv::blur(bg, bg, cv::Size{3, 3});
		}*/

		//gen.generateCropped(roiBuffer, bg, imgs.at(distI(rng)));

		//cv::resize(bg, bg, size);
		imwrite(Utils::out + classID + std::to_string(imgCounter) + Utils::imageExt, bg);

		i = (i == (imgs.size() - 1)) ? 0 : i+1;
	}

	// Free resources
	cv::destroyAllWindows();

	return 0;
}