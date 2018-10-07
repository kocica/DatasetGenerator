/**
 *
 */

#include "ROI.hpp"
#include "Generator.hpp"

int main()
{
	int         imgClass          = 0;
	std::string pathToBackgrounds = "./img/",
				pathToImages      = "./img/",
				backgroundName    = "background.jpg",
				imageName         = "traffic_sign.png",
				csvName           = "./annotations/testAnnot.txt";
	cv::Mat     background,
				trainingImg;
 
	// Load images
	background  = cv::imread(pathToBackgrounds + backgroundName, CV_LOAD_IMAGE_COLOR);
	trainingImg = cv::imread(pathToImages + imageName, CV_LOAD_IMAGE_COLOR);
 
	// Output annotation file
	std::ofstream annotFile(csvName);
 
	// Regions of interest selection
	ROI_buffer roiBuffer;
	roiBuffer = getRegionsOfInterest(background);
 
	// Image & annotation generator
	DtstGenerator gen(annotFile, imgClass);
	gen.generate(roiBuffer, background, trainingImg);
 
	// Show image
	cv::imshow("bbox", background);
 
	// Wait for user & free resources
	cv::waitKey(0);
	cv::destroyAllWindows();
	return 0;
}