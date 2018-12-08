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
	///////////////////////////////////////////////////////////////////////////////////////////////
	// Locals

	int               imgClass   = 0;
	int               imgCounter = 0;
	int               ret        = 0;
	int               w          = 0;
	int               h          = 0;
	size_t            i          = 0;
	constexpr size_t  nGenImgs   = 2;

	std::string       pathToBackgrounds, pathToImages, backgroundName, imageName, csvName, classID;

	Utils::ImgBuffer  bgs;
	Utils::ImgBuffer  imgs;

	Utils::StrBuffer  dirs;

	cv::Size          outImgSize = std::move( cv::Size{ 0, 0 } );

	cv::Mat           bg;
	cv::Mat           img;

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Parse arguments

	if ((ret = Utils::parseArgs(argc, argv, pathToBackgrounds, pathToImages)) != 0)
	{
		return ret;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Load images & directories of classes

	Utils::loadImages(pathToBackgrounds, bgs,  cv::IMREAD_COLOR);
	Utils::getDirectories(pathToImages, dirs);


	///////////////////////////////////////////////////////////////////////////////////////////////
	// Regions of interest selection

	ROI_buffer roiBuffer;

#	ifdef ROI_SELECTION
		cv::Mat exampleBg = cv::imread("data/roi-selection.png");
		cv::resize(exampleBg, exampleBg, outImgSize);                      // Size from input args
		roiBuffer = getRegionsOfInterest(exampleBg);
#	endif


	///////////////////////////////////////////////////////////////////////////////////////////////
	// RNG

	std::mt19937 rng;
	rng.seed(std::random_device()());

	std::uniform_int_distribution<std::mt19937::result_type> distX = std::uniform_int_distribution<std::mt19937::result_type> (0, bgs.at(0).cols - 450);
	std::uniform_int_distribution<std::mt19937::result_type> distY = std::uniform_int_distribution<std::mt19937::result_type> (0, bgs.at(0).rows - 350);
	std::uniform_int_distribution<std::mt19937::result_type> distI = std::uniform_int_distribution<std::mt19937::result_type> (0, imgs.size() - 1);

#	ifdef RANDOM_W_H
		std::uniform_int_distribution<std::mt19937::result_type> distW = std::uniform_int_distribution<std::mt19937::result_type> (225, 300);
		std::uniform_int_distribution<std::mt19937::result_type> distH = std::uniform_int_distribution<std::mt19937::result_type> (150, 200);
		std::uniform_int_distribution<std::mt19937::result_type> distB = std::uniform_int_distribution<std::mt19937::result_type> (0, 20);
#	endif

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Get image width & height

#	ifdef RANDOM_W_H
		w = distW(rng);
		h = distH(rng);

		if (distB(rng) < 3)
		{
			std::swap(w, h);
		}
#	else
		w = 407;
		h = 309;
#	endif

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Generate N images for each class

	for (auto& path : dirs)
	{
		imgs.clear();

#		ifdef IMG_CROPPED
			Utils::loadImages(path, imgs, cv::IMREAD_UNCHANGED);  // With alpha-channel
#		else
			Utils::loadImages(path, imgs, cv::IMREAD_COLOR);
#		endif

		int nBackgrounds = bgs.size();
		int nImages      = imgs.size();

		classID  = std::experimental::filesystem::path(path).filename();
		imgClass = Utils::getImgClass(path);

		for (imgCounter = 0; imgCounter < nGenImgs; imgCounter++)
		{
			bgs.at(imgCounter % nBackgrounds).copyTo(bg);

			cv::Rect roi { (int)distX(rng), (int)distY(rng), w, h };
			bg  = bg(roi);

#			ifdef IMG_CROPPED
				imgs.at(i).copyTo(img);
#			else
				imgs.at(distI(rng)).copyTo(img);
#			endif

			// Output annotation file
			std::ofstream annotFile(Utils::out + classID + std::to_string(imgCounter) + Utils::annotExt);

			// Image & annotation generator
			DtstGenerator gen(annotFile, imgClass);

			// Generate images and annotations
#			ifdef IMG_CROPPED
				gen.generate(roiBuffer, bg, img);
#			else
				gen.generateCropped(roiBuffer, bg, img);
#			endif

#			ifdef ROI_SELECTION
				cv::resize(bg, bg, outImgSize);
#			endif

			// Save image
			imwrite(Utils::out + classID + std::to_string(imgCounter) + Utils::imageExt, bg);

			// Move to next image or start from the beginning
			i = (i == (nImages - 1)) ? 0 : i + 1;
		}
	}

	// Destroy OpenCV windows
	cv::destroyAllWindows();

	return 0;
}