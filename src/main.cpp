/**
 *
 */

#include "ROI.hpp"
#include "Generator.hpp"

int main()
{
	cv::Mat background, training_img;

	background   = cv::imread("./../img/background.jpg", CV_LOAD_IMAGE_COLOR);
	training_img = cv::imread("./../img/traffic_sign.png", CV_LOAD_IMAGE_COLOR);

	//Rect2d r = selectROI(background);

  ROI_buffer roiBuffer;
  roiBuffer = getRegionsOfInterest(background);

	DtstGenerator gen;
	gen.generate(roiBuffer, background, training_img);

	cv::imshow("bbox", background);

	cv::waitKey(0);
	cv::destroyAllWindows();
	return 0;
}
