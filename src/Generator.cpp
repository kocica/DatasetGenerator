/**
 *
 */
 
#include "Generator.hpp"
 
DtstGenerator::DtstGenerator(std::ofstream& out, int imgClass)
: m_out { out }
, m_class { imgClass }
{
  srand(time(NULL));
}
 
DtstGenerator::~DtstGenerator()
{
 
}
 
void DtstGenerator::generate(std::vector<std::pair<cv::Point, cv::Point>>& b, cv::Mat& m, cv::Mat& m2)
{
  for (auto& it : b)
  {
    cv::Mat tmpImg;
    m2.copyTo(tmpImg);
 
    int max_x = std::max(it.first.x, it.second.x);
    int min_x = std::min(it.first.x, it.second.x);
    int max_y = std::max(it.first.y, it.second.y);
    int min_y = std::min(it.first.y, it.second.y);
    int dif_x = max_x - min_x;
    int dif_y = max_y - min_y;
 
    int x = rand() % dif_x + min_x;
    int y = rand() % dif_y + min_y;
 
    // Resize image due to its position in background
    resize(tmpImg, x, m.cols/2);
 
    // Save data to annotation file
    createAnnotation(m, tmpImg, x, y);
 
    // Copy image to background
    if (((x + tmpImg.cols) <= m.cols) && ((y + tmpImg.rows) <= m.rows))
    {
      tmpImg.copyTo( m (cv::Rect {x, y, tmpImg.cols, tmpImg.rows} ) );
    }
    else
    {
      // Shift image left
      if ((x + tmpImg.cols) > m.cols)
      {
        x -= (x + tmpImg.cols) - m.cols;
      }
      // Shift image up
      if ((y + tmpImg.rows) > m.rows)
      {
        y -= (y + tmpImg.rows) - m.rows;
      }

      tmpImg.copyTo( m (cv::Rect {x, y, tmpImg.cols, tmpImg.rows} ) );
    }
  }
}
 
void DtstGenerator::createAnnotation(cv::Mat& m, cv::Mat& m2, int& x, int& y)
{
  // Save image class
  m_out << m_class << " ";
  // Save relative position
  m_out << (x + ((double) m2.cols / 2)) / m.cols << " ";
  m_out << (y + ((double) m2.rows / 2)) / m.rows << " ";
  // Save relative proportion
  m_out << (double) m2.cols / m.cols << " ";
  m_out << (double) m2.rows / m.rows << std::endl;
 
  /*
  // DEBUG, TODO: Remove
  int x2     = ((x + ((double) m2.cols / 2)) / m.cols) * m.cols - ((double) m2.cols / 2);
  int y2     = ((y + ((double) m2.rows / 2)) / m.rows) * m.rows - ((double) m2.rows / 2);
  int size_x = ((double) m2.cols / m.cols) * m.cols;
  int size_y = ((double) m2.rows / m.rows) * m.rows;
 
  cv::Point pt{x2, y2};
  cv::Point pt2{x2 + size_x, y2 + size_y};
 
  cv::rectangle(m, pt, pt2, cv::Scalar{0, 255, 0});
  */
}
 
void DtstGenerator::resize(cv::Mat& m, int pos, int mid)
{
  double ratio = 0;
 
  if (pos > mid)
  {
    ratio = (pos - mid) / (double) mid;
  }
  else
  {
    ratio = 1 - (pos / (double) mid);
  }
 
  /*
  // DEBUG, TODO: Remove
  std::cout << "pos:     " << pos                    << std::endl
            << "mid:     " << mid                    << std::endl
            << "m.rows:  " << m.rows                 << std::endl
            << "m.cols:  " << m.cols                 << std::endl
            << "ratio:   " << ratio                  << std::endl
            << "ratio r: " << (int) (ratio * m.rows) << std::endl
            << "ratio c: " << (int) (ratio * m.cols) << std::endl
            ;
  */
 
  cv::resize(m, m, cv::Size{(int) (ratio * m.rows), (int) (ratio * m.cols)});
}