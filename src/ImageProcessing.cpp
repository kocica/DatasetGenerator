/**
 * @file    ImageProcessing.hpp
 * @author  Filip Kocica <xkocic01@stud.fit.vutbr.cz>
 * @date    13/10/2018
 *
 * Training dataset generator
 *
 * This file provides image processing operations, such as 3D transformations
 */

#include "ImageProcessing.hpp"

namespace ImageProcessing
{
    void rotateImage(const cv::Mat &input, cv::Mat &output, double alpha, double beta, double gamma, double dx, double dy, double dz, double f)
    {
        alpha = (alpha - 90.) * CV_PI/180.;
        beta  = (beta - 90.)  * CV_PI/180.;
        gamma = (gamma - 90.) * CV_PI/180.;

        // get width and height for ease of use in matrices
        double w = (double)input.cols;
        double h = (double)input.rows;

        // Projection 2D -> 3D matrix
        cv::Mat A1 = (cv::Mat_<double>(4,3) <<
                    1, 0, -w/2,
                    0, 1, -h/2,
                    0, 0,    0,
                    0, 0,    1);

        // Rotation matrices around the X, Y, and Z axis
        cv::Mat RX = (cv::Mat_<double>(4, 4) <<
                    1,          0,           0, 0,
                    0, cos(alpha), -sin(alpha), 0,
                    0, sin(alpha),  cos(alpha), 0,
                    0,          0,           0, 1);

        cv::Mat RY = (cv::Mat_<double>(4, 4) <<
                    cos(beta), 0, -sin(beta), 0,
                    0, 1,          0, 0,
                    sin(beta), 0,  cos(beta), 0,
                    0, 0,          0, 1);

        cv::Mat RZ = (cv::Mat_<double>(4, 4) <<
                    cos(gamma), -sin(gamma), 0, 0,
                    sin(gamma),  cos(gamma), 0, 0,
                    0,          0,           1, 0,
                    0,          0,           0, 1);

        // Composed rotation matrix with (RX, RY, RZ)
        cv::Mat R = RX * RY * RZ;

        // Translation matrix
        cv::Mat T = (cv::Mat_<double>(4, 4) <<
                    1, 0, 0, dx,
                    0, 1, 0, dy,
                    0, 0, 1, dz,
                    0, 0, 0, 1);

        // 3D -> 2D matrix
        cv::Mat A2 = (cv::Mat_<double>(3,4) <<
                    f, 0, w/2, 0,
                    0, f, h/2, 0,
                    0, 0,   1, 0);

        // Final transformation matrix
        cv::Mat trans = A2 * (T * (R * A1));

        // Apply matrix transformation
        cv::warpPerspective(input, output, trans, input.size(), cv::INTER_LANCZOS4);
    }

    void copy2bg(cv::Mat& bg, cv::Mat& img, cv::Mat& alpha, int& x, int& y, int& b, int& v)
    {
        int sizex = img.cols;
        int sizey = img.rows;
        cv::Mat subImg = bg(cv::Range(y, y + sizey), cv::Range(x, x + sizex));

        img.convertTo(img, CV_32FC3);
        subImg.convertTo(subImg, CV_32FC3);

        cv::cvtColor(alpha, alpha, cv::COLOR_GRAY2BGR);
        alpha.convertTo(alpha, CV_32FC3, 1.0/255);

        if (b)
        {
            subtract(img, v, img);
        }
        else
        {
            add(img, v, img);
        }

        multiply(alpha, img, img);

        cv::Mat ouImage = cv::Mat::zeros(img.size(), img.type());

        // Multiply the foreground with the alpha matte
        multiply(alpha, img, img);

        // Multiply the background with ( 1 - alpha )
        multiply(cv::Scalar::all(1.0)-alpha, subImg, subImg);

        // Add the masked foreground and background.
        add(img, subImg, ouImage);

        ouImage.copyTo(bg(cv::Rect(x, y, sizex, sizey)));
    }

    void resize(cv::Mat& m, int pos, int mid)
    {
        double ratio = 0;

        if (pos > mid)
        {
            ratio = (pos - mid) / (double) mid;
        }
        else
        {
            ratio  = 1 - (pos / (double) mid);
            ratio *= 0.5;
        }

        cv::resize(m, m, cv::Size{(int) (ratio * m.rows), (int) (ratio * m.cols)});
    }


    void rotateAngle(cv::Mat& img, double angle)
    {
        double width  = img.size().width;
        double height = img.size().height;

        cv::Point2d center = cv::Point2d (width / 2, height / 2);
        cv::Rect bounds    = cv::RotatedRect (center, img.size(), angle).boundingRect();
        cv::Mat resized    = cv::Mat::zeros (bounds.size(), img.type());

        double offsetX = (bounds.width - width) / 2;
        double offsetY = (bounds.height - height) / 2;

        cv::Rect roi = cv::Rect (offsetX, offsetY, width, height);
        img.copyTo (resized (roi));
        center += cv::Point2d (offsetX, offsetY);

        cv::Mat M = cv::getRotationMatrix2D (center, angle, 1.0);
        cv::warpAffine (resized, resized, M, resized.size());

        resized.copyTo(img);
    }
}