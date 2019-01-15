/**
 * @file    ImageProcessing.hpp
 * @author  Filip Kocica <xkocic01@stud.fit.vutbr.cz>
 * @date    13/10/2018
 *
 * Training dataset generator
 *
 * This file provides image processing operations, such as 3D transformations, resizing, etc.
 */

#include "ImageProcessing.hpp"

/**
 * @brief Namespace with image processing functions
 */
namespace ImageProcessing
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
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

    ///////////////////////////////////////////////////////////////////////////////////////////////
    void copy2bg(cv::Mat& bg, cv::Mat& img, cv::Mat& alpha, const int& x, const int& y)
    {
        int sizex = img.cols;
        int sizey = img.rows;
        cv::Mat subImg = bg(cv::Range(y, y + sizey), cv::Range(x, x + sizex));

        img.convertTo(img, CV_32FC3);
        subImg.convertTo(subImg, CV_32FC3);

        cv::cvtColor(alpha, alpha, cv::COLOR_GRAY2BGR);
        alpha.convertTo(alpha, CV_32FC3, 1.0/255);

        multiply(alpha, img, img);

        cv::Mat ouImage = cv::Mat::zeros(img.size(), img.type());

        // Multiply the foreground with the alpha matte
        multiply(alpha, img, img);

        // Multiply the background with ( 1 - alpha )
        multiply(cv::Scalar::all(1.0) - alpha, subImg, subImg);

        // Add the masked foreground and background.
        add(img, subImg, ouImage);

        ouImage.copyTo(bg(cv::Rect(x, y, sizex, sizey)));
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    void copy2bgCropped(cv::Mat& bg, cv::Mat& img, cv::Mat& annot, const int& x, const int& y,
						const int& x2, const int& y2, const bool& useMask)
    {
#       ifdef SEAMLESS_CLONE        

            cv::Mat imgMask;

            if (useMask)
            {
                imgMask = cv::Mat::zeros(img.rows, img.cols, img.depth());

                const int diffx = x2 - x;
                const int diffy = y2 - y;
                const int sizex = annot.cols;
                const int sizey = annot.rows;

                ////////////// Method 1 ////////////// (More suitable for more than 4 points)
                /*cv::Point poly[1][4];
                poly[0][0] = cv::Point(diffx, diffy);
                poly[0][1] = cv::Point(diffx + sizex, diffy);
                poly[0][2] = cv::Point(diffx, diffy + sizey);
                poly[0][3] = cv::Point(diffx + sizex, diffy + sizey);

                const cv::Point* polygons[1] = { poly[0] };
                int numPoints[] = { 4 };

                cv::fillPoly(imgMask, polygons, numPoints, 1, cv::Scalar(255, 255, 255));*/

                ////////////// Method 2 //////////////
                /*for (int i = 0; i < imgMask.rows; i++)
                {
                    for (int j = 0; j < imgMask.cols; j++)
                    {
                        if (i > diffx && i < (diffx + sizex)
                            && j > diffy && j < (diffy + sizey))
                        {
                            imgMask.at<cv::Vec3b>(i, j) = cv::Vec3b{ 255, 255, 255 };
                        }
                    }
                }*/

                ////////////// Method 3 ////////////// (Probably most optimized)
                cv::rectangle(imgMask, cv::Point(diffx, diffy), cv::Point(diffx + sizex, diffy + sizey),
                            cv::Scalar(255, 255, 255),  CV_FILLED);
            }
            else
            {
                imgMask = 255 * cv::Mat::ones(img.rows, img.cols, img.depth());
            }

            // The location of the center of the img in the bg
            cv::Point center( x + img.cols/2, y + img.rows/2 );

            // Seamlessly clone img into bg and put the results in output
            cv::Mat clone = bg;
            cv::seamlessClone(img, bg, imgMask, center, clone, cv::NORMAL_CLONE); /* MIXED_CLONE */
            bg = std::move(clone);
#       else
            img.copyTo(bg(cv::Rect(x, y, img.cols, img.rows)));
#       endif
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    double resize(cv::Mat& m, int pos, int mid, int r)
    {
        double ratio = r / 100.;
        double rows = m.rows;

#       ifdef REALISTIC_SIZE
            // Resize image due to its position in background
            // ie. closer to the middle, smaller image will be

            if (pos > mid)
            {
                ratio = (pos - mid) / (double) mid;
            }
            else
            {
                ratio  = 1 - (pos / (double) mid);
                ratio *= 0.5;
            }

            cv::resize( m, m, cv::Size{ (int) (ratio * 100), (int) (ratio * 100) } );
#       else
            // Resize image to approtimate size

            // TODO: Replace magic constants !!!
            double diffSize = 407 / 309; //m.cols / (double) m.rows;

            cv::resize( m, m, cv::Size{ (int) (ratio * 50), (int) (ratio * diffSize * 50) } );
#       endif

        return (m.rows / rows);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    void rotateAngle(cv::Mat& img, double angle)
    {
        double width  = img.size().width;
        double height = img.size().height;

        cv::Point2d center = cv::Point2d(width / 2, height / 2);
        cv::Rect bounds    = cv::RotatedRect(center, img.size(), angle).boundingRect();
        cv::Mat resized    = cv::Mat::zeros(bounds.size(), img.type());

        double offsetX = (bounds.width - width)   / 2;
        double offsetY = (bounds.height - height) / 2;

        cv::Rect roi = cv::Rect(offsetX, offsetY, width, height);
        img.copyTo (resized(roi));
        center += cv::Point2d(offsetX, offsetY);

        cv::Mat M = cv::getRotationMatrix2D(center, angle, 1.0);
        cv::warpAffine(resized, resized, M, resized.size());

        resized.copyTo(img);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    void modifyLuminescence(cv::Mat& img, const double& alpha, const int& beta)
    {
        cv::Mat newImage = cv::Mat::zeros( img.size(), img.type() );

        // Do the operation for each pixel: newImage(i,j) = alpha*img(i,j) + beta

        /*
        for( int y = 0; y < img.rows; y++ )
        {
            for( int x = 0; x < img.cols; x++ )
            {
                for( int c = 0; c < 3; c++ )
                {
                    newImage.at<cv::Vec3b>(y, x)[c] = cv::saturate_cast<uchar>( alpha * ( img.at<cv::Vec3b>(y, x)[c] ) + beta );
                }
            }
        }
        */

        // Probably more optimized method
        img.convertTo(newImage, -1, alpha, beta);

        img = std::move( newImage );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    void gammaCorrection(cv::Mat& img, const double& gamma)
    {
        cv::Mat lookUpTable( 1, 256, CV_8U );
        uchar* p = lookUpTable.ptr();

        for( int i = 0; i < 256; ++i )
        {
            p[i] = cv::saturate_cast<uchar>( pow( i / 255.0, gamma ) * 255.0 );
        }

        cv::Mat res = img.clone();
        cv::LUT( img, lookUpTable, res );

        img = std::move( res );
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Hue intervals @see http://answers.opencv.org/question/93899/hsv-range-for-rubiks-cube-color/
    //
    /** @brief Says whether hue value is inside Red hue interval */
    static bool isRedHue(const uchar& hue)    { return (( hue >=   0 && hue <=   9 ) || ( hue >= 177 && hue <= 180 )); }
    /** @brief Says whether hue value is inside Blue hue interval */
    static bool isBlueHue(const uchar& hue)   { return  ( hue >= 101 && hue <= 150 ); }
    /** @brief Says whether hue value is inside Yellow hue interval */
    static bool isYellowHue(const uchar& hue) { return  ( hue >=  16 && hue <=  45 ); }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    void modifyHue(cv::Mat& img)
    {
        std::mt19937 rng;
        rng.seed(std::random_device()());

        PRNG::Uniform_t probDist{1, 10};
        PRNG::Uniform_t hueDistRedLow{0, 9};
        PRNG::Uniform_t hueDistRedHigh{177, 180};
        PRNG::Uniform_t hueDistBlue{104, 109};
        PRNG::Uniform_t hueDistYellow{16, 45};

        uchar newHueRed;
        if ( probDist( rng ) <= 5 )  // 50%
            newHueRed      = hueDistRedLow( rng );  //   0 -   9
        else                         // 50%
            newHueRed      = hueDistRedHigh( rng ); // 177 - 180

        uchar newHueBlue   = hueDistBlue( rng );
        uchar newHueYellow = hueDistYellow( rng );

        cv::Mat hsv;

        // Convert sign to HSV
        cv::cvtColor(img, hsv, CV_BGR2HSV);

        // For each pixel

        cv::MatIterator_<cv::Vec3b> begin = hsv.begin<cv::Vec3b>();
        cv::MatIterator_<cv::Vec3b> end   = hsv.end<cv::Vec3b>();

        std::for_each(begin, end, [&](cv::Vec3b& p)
        {
            // H = 0; S = 1; V = 2
            // Get Hue (8-bit unsigned integer)
            uchar& h = p[0];

            // TODO: Gen value for each pixel separately -- hueDistRed( rng ) ???

            if ( isRedHue( h ) ) // Red pixel
            {
                h = newHueRed;
            }
            else if ( isBlueHue( h ) ) // Blue pixel
            {
                h = newHueBlue;
            }
            else if ( isYellowHue( h ) ) // Yellow pixel
            {
                h = newHueYellow;
            }
        });

        // Convert sign back to BGR
        cv::cvtColor(hsv, img, CV_HSV2BGR);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    double getImgBrightness(const cv::Mat& img)
    {
        std::vector<cv::Mat> channels;
        cv::Mat tmp;
        cv::Mat lum;

        img.copyTo(tmp);

        split(tmp, channels);

        // TODO: Its BGR, not RGB - so maybe change indexes
        //       0,1,2 -> 2,1,0

        // Convert image to YUV format
        // Y = 0.299 * R
        channels[0] = channels[0] * 0.299;
        // U = 0.587 * G
        channels[1] = channels[1] * 0.587;
        // V = 0.144 * B
        channels[2] = channels[2] * 0.114;

        lum = channels[0] + channels [1] + channels[2];

        cv::Scalar sum = cv::sum(lum);

        // Percentage conversion factor
        return (sum[0] / ((pow(2, 8) - 1) * img.rows * img.cols) * 2);
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    void gaussianNoise(cv::Mat& img)
    {
        cv::Mat gaussNoise = img.clone();
        cv::randn(gaussNoise, 1, 15);

        img = img + gaussNoise;
    }

	///////////////////////////////////////////////////////////////////////////////////////////////
	void saltNPepperNoise(cv::Mat& img)
    {
        cv::Mat saltpepperNoise = cv::Mat::zeros(img.rows, img.cols, CV_8U);
        randu(saltpepperNoise, 0, 255);

        cv::Mat black = saltpepperNoise < 2;
        cv::Mat white = saltpepperNoise > 253;

        img.setTo(255, white);
        img.setTo(0, black);
    }
}