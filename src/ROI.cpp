/**
 *
 */

#include "ROI.hpp"

ROI_buffer   buffer;
cv::Point    pt;
cv::Mat      img;

void mouseClick(int event, int x, int y, int flags, void *param)
{
  switch(event)
  {
     case CV_EVENT_LBUTTONDOWN:
     {
        pt.x = x;
        pt.y = y;
        break;
     }

     case CV_EVENT_LBUTTONUP:
     {
        cv::Point pt2{x, y};

        cv::Mat roi(img, cv::Rect{pt, pt2});

        //cv::imshow("Clone",roi);

        cv::rectangle(img, pt, pt2, cv::Scalar{0, 255, 0});
        cv::imshow("bbox", img);

        // Store coords to the buffer
        buffer.emplace_back(std::make_pair(pt, pt2));

        break;
     }
   }
}

ROI_buffer getRegionsOfInterest(cv::Mat& m)
{
  m.copyTo(img);
  cv::namedWindow("bbox", 1);

  // Click event handler
	cv::setMouseCallback("bbox", mouseClick, 0);

  cv::imshow("bbox", img);

  cv::waitKey(0); // TODO: Dokud nezmackne urcitou klavesu, napsat poznamku

  // Remove click event handler
  cv::setMouseCallback("bbox", NULL, NULL);

  m = img;
  return buffer;
}
