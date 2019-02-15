/**
 * @file    Utils.hpp
 * @author  Filip Kocica <xkocic01@stud.fit.vutbr.cz>
 * @date    12/10/2018
 *
 * Training dataset generator
 *
 * Utilities
 */

#pragma once

// STD, STL
#include <iostream>
#include <vector>
#include <experimental/filesystem>
#include <fstream>
#include <string>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

/**
 * @brief Namespace which contains utils implementation
 */
namespace Utils
{
    /** @brief Buffer of images */
    using ImgBuffer = std::vector<cv::Mat>;
    /** @brief Buffer of strings */
    using StrBuffer = std::vector<std::string>;

    const std::string antExt      = ".txt";
    const std::string imgExt      = ".jpg";
    const std::string outDir      = "out/";
    const std::string imgClassFn  = "imgClass";
    const std::string annotation  = "annotation.csv";

    /************************************************************************************
     * @brief Functor to read first number from annotation text file ie. number of the class
     *
     * @param [in] path Where are class directory & annotation file located
     * @return Class number
     */
    auto getImgClass = [&](const std::string& path) -> int
    {
        std::fstream fs { path + "/" + imgClassFn, std::ios_base::in };
        int ret; fs >> ret; return(ret);
    };

    /************************************************************************************
     * @brief Directories are named by classes they represents, so this functor extracts name of dir
     *
     * @param [in] path Path to directory
     * @return Class ID
     */
    auto getClassID = [&](const std::string& path) -> std::string
    {
        return (std::experimental::filesystem::path(path).filename());
    };

    /************************************************************************************
     * @brief From specified path (directory) loads all images to buffer
     * 
     * @param [in] ext Extension of images
     * @param [in] path Directory
     * @param [out] imgs Buffer of loaded images
     * @param [in] mode Mode of imread
     */
    void loadImages(const std::string& ext, const std::string& path, ImgBuffer& imgs, const int& mode);

    /************************************************************************************
     * @brief Overload of function above, saving also names of images
     * 
     * @param [in] ext Extension of images
     * @param [in] path Directory
     * @param [out] imgs Buffer of loaded images
     * @param [out] names Buffer of names of loaded images
     * @param [in] mode Mode of imread
     */
    void loadImages(const std::string& ext, const std::string& path, ImgBuffer& imgs, StrBuffer& names, const int& mode);

    /************************************************************************************
     * @brief Arguments parser
     * 
     * @param [in] argc Number of arguments
     * @param [in] argv Argument values
     * @param [out] pathBgs Directory with backgrounds
     * @param [out] pathImg Directory with images placed into bacgrounds
     */
    int parseArgs(int argc, char **argv, std::string& pathBgs, std::string& pathImgs);

    /************************************************************************************
     * @brief Returns buffer of strings with all directories in path
     * 
     * @param [in] path Directory where we're lookin for directories
     * @param [out] strBuffer Buffer of strings with directories in path
     */
    void getDirectories(const std::string& path, StrBuffer& strBuffer);

    /************************************************************************************
     * @brief Returns buffer of strings with all annotations in annotation csv file
     * 
     * @param [in] path Path to annotation file
     * @param [out] strBuffer Buffer of annotations
     */
    void readAnnotations(const std::string& path, StrBuffer& strBuffer);

    /************************************************************************************
     * @brief Prints program usage
     */
    void printUsage();

    /************************************************************************************
     * @brief Swap values of params
     *
     * @param [in,out] Value to be swapped
     * @param [in,out] Value to be swapped
     */
    template<typename T>
    inline void swap(T& first, T& second)
    {
        T tmp  = first;
        first  = second;
        second = tmp;
    }
}