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
#include "GeneratorTransparent.hpp"
#include "GeneratorCropped.hpp"
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
	constexpr size_t  nGenImgs   = 10;

	std::string       pathToBackgrounds, pathToImages, backgroundName, imageName, csvName, classID;

	Utils::ImgBuffer  bgs;
	Utils::ImgBuffer  imgs;

	Utils::StrBuffer  dirs;

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
	// RNG

	std::mt19937 rng;
	rng.seed(std::random_device()());

	PRNG::Uniform distX = PRNG::Uniform(0, bgs.at(0).cols - 450);
	PRNG::Uniform distY = PRNG::Uniform(0, bgs.at(0).rows - 350);
	PRNG::Uniform distI;
	
#	ifdef RANDOM_W_H
		PRNG::Uniform distW = PRNG::Uniform(225, 300);
		PRNG::Uniform distH = PRNG::Uniform(150, 200);
		PRNG::Uniform distB = PRNG::Uniform(0, 20);
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
	// Regions of interest selection

	ROI_buffer roiBuffer;

#	ifdef ROI_SELECTION
		cv::Mat exampleBg = cv::imread("data/roi-selection.png");
		cv::resize(exampleBg, exampleBg, cv::Size{ w, h });
		roiBuffer = getRegionsOfInterest(exampleBg);
#	endif

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Generate N images for each class

	for (auto& path : dirs)
	{
		imgs.clear();

#		ifdef IMG_TRANSPARENT
			Utils::loadImages(path, imgs, cv::IMREAD_UNCHANGED);  // With alpha-channel
#		else
			Utils::loadImages(path, imgs, cv::IMREAD_COLOR);      // Classification dataset TS
#		endif

		int nBackgrounds = bgs.size();
		int nImages      = imgs.size();

		classID  = std::experimental::filesystem::path(path).filename();
		imgClass = Utils::getImgClass(path);

		i = 0;
		distI = PRNG::Uniform(0, imgs.size() - 1);

		for (imgCounter = 0; imgCounter < nGenImgs; imgCounter++)
		{
			bgs.at(imgCounter % nBackgrounds).copyTo(bg);

			cv::Rect roi { (int)distX(rng), (int)distY(rng), w, h };
			bg  = bg(roi);

#			ifdef IMG_TRANSPARENT
				imgs.at(i).copyTo(img);
#			else
				imgs.at(distI(rng)).copyTo(img);
#			endif

			// Output annotation file passed to generator
			std::ofstream annotFile(Utils::out + classID + std::to_string(imgCounter) + Utils::annotExt);

			// Image & annotation generator
			// TODO: Unique ptr
			DatasetGenerator_t *generator;

			// Generate images and annotations
#			ifdef IMG_TRANSPARENT
				generator = new DatasetGeneratorTransparent_t{annotFile, imgClass};
#			else
				generator = new DatasetGeneratorCropped_t{annotFile, imgClass};
#			endif

			generator->generateDataset(roiBuffer, bg, img);

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