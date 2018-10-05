/**
 *
 */

#include "Generator.hpp"


void DtstGenerator::generate(std::vector<std::pair<cv::Point, cv::Point>>& b, cv::Mat& m, cv::Mat& m2)
{
  srand(time(NULL));

  for (auto& it : b)
  {
    int max_x = std::max(it.first.x, it.second.x);
    int min_x = std::min(it.first.x, it.second.x);
    int max_y = std::max(it.first.y, it.second.y);
    int min_y = std::min(it.first.y, it.second.y);
    int dif_x = max_x - min_x;
    int dif_y = max_y - min_y;

    int x = rand() % dif_x + min_x;
    int y = rand() % dif_y + min_y;

    if (((x + m2.cols) <= m.cols) && ((y + m2.rows) <= m.rows))
    {
      // Try to shift image to left side
      m2.copyTo(m(cv::Rect(x, y, m2.cols, m2.rows)));
    }
  }
}
