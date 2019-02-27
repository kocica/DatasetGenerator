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
#ifdef ROI_SELECTION
#include "ROI.hpp"
#endif

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

	size_t            randNum    = 0;
	size_t            i          = 0;
	constexpr size_t  nGenImgs   = 10;

	std::string       pathToBackgrounds;
	std::string       pathToImages;
	std::string       classID;

	Utils::ImgBuffer  bgs;
	Utils::ImgBuffer  imgs;

	Utils::StrBuffer  dirs;
#	ifndef IMG_TRANSPARENT
	Utils::StrBuffer  tsAnnotations;
	Utils::StrBuffer  imgNames;
#	endif

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

	Utils::loadImages("png", pathToBackgrounds, bgs,  cv::IMREAD_COLOR);
	Utils::getDirectories(pathToImages, dirs);

	///////////////////////////////////////////////////////////////////////////////////////////////
	// RNG

	std::mt19937 rng;
	rng.seed(std::random_device()());

	PRNG::Uniform_t distX = PRNG::Uniform_t{0, (size_t) bgs.at(0).cols - 450};
	PRNG::Uniform_t distY = PRNG::Uniform_t{0, (size_t) bgs.at(0).rows - 350};
	PRNG::Uniform_t distI;
	
#	ifdef RANDOM_W_H
		PRNG::Uniform_t distW = PRNG::Uniform_t{225, 300};
		PRNG::Uniform_t distH = PRNG::Uniform_t{150, 200};
		PRNG::Uniform_t distB = PRNG::Uniform_t{0, 20};
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

#	ifdef ROI_SELECTION
		ROIBuffer_t roiBuffer;
		cv::Mat     exampleBg;

		exampleBg = cv::imread("data/roi-selection.png");
		cv::resize(exampleBg, exampleBg, cv::Size{ w, h });
		roiBuffer = getRegionsOfInterest(exampleBg);
#	endif

	///////////////////////////////////////////////////////////////////////////////////////////////
	// Iterate over all TS classes in given directory, load TS from each of them

	for (auto& path : dirs)
	{
		i = 0;
		imgs.clear();
#		ifndef IMG_TRANSPARENT
		imgNames.clear();
		tsAnnotations.clear();
#		endif

#		ifdef IMG_TRANSPARENT
			Utils::loadImages("png", path, imgs, cv::IMREAD_UNCHANGED);       // With alpha-channel
#		else
			Utils::loadImages("ppm", path, imgs, imgNames, cv::IMREAD_COLOR); // Cropped classification TS dataset
#		endif

		size_t nBackgrounds = bgs.size();
		size_t nImages      = imgs.size();

		if (nBackgrounds == 0 || nImages == 0)
		{
			std::cerr << "E: Failed to load images." << std::endl;
			return 1; 
		}

		classID  = Utils::getClassID(path);
		imgClass = Utils::getImgClass(path);

#		ifndef IMG_TRANSPARENT
			Utils::readAnnotations(path, tsAnnotations);
#		endif

		distI = PRNG::Uniform_t{0, nImages - 1};

		///////////////////////////////////////////////////////////////////////////////////////////////
		// Generate `nGenImgs` images for each of the TS classes

		for (imgCounter = 0; imgCounter < nGenImgs; imgCounter++)
		{
			bgs.at(imgCounter % nBackgrounds).copyTo(bg);

			// Select random part of the background
			cv::Rect roi { (int)distX(rng), (int)distY(rng), w, h };
			bg  = bg(roi);

#			ifdef IMG_TRANSPARENT
				imgs.at(i).copyTo(img);
#			else
				randNum = distI(rng);
				imgs.at(randNum).copyTo(img);
#			endif

			// Create output annotation file passed to generator
			std::ofstream annotFile(Utils::outDir + classID + std::to_string(imgCounter) + Utils::antExt);

			// Image & annotation generator
			DatasetGenerator_t* generator;

			// Generate image with annotations
#			ifdef IMG_TRANSPARENT
				generator = new DatasetGeneratorTransparent_t{annotFile, imgClass};
#			else
				generator = new DatasetGeneratorCropped_t{annotFile, imgClass, tsAnnotations, imgNames.at(randNum)};
#			endif

#			ifdef ROI_SELECTION
				generator->generateDataset(roiBuffer, bg, img);
#			else
				generator->generateDataset(bg, img);
#			endif

			// Save image
			imwrite(Utils::outDir + classID + std::to_string(imgCounter) + Utils::imgExt, bg);

			// Move to next TS or start from the beginning
			i = (i == (nImages - 1)) ? 0 : i + 1;
		}
	}

	// Destroy OpenCV windows if exists
#	ifdef GENERATOR_DEBUG
		cv::destroyAllWindows();
#	endif

	return 0;
}