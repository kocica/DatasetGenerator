/**
 * @file    Utils.cpp
 * @author  Filip Kocica <xkocic01@stud.fit.vutbr.cz>
 * @date    12/10/2018
 *
 * Training dataset generator
 *
 * Utilities
 */

#include "Utils.hpp"

namespace Utils
{
    ///////////////////////////////////////////////////////////////////////////////////////////////
    int parseArgs(int argc, char **argv, std::string& pathBgs, std::string& pathImgs)
    {
        if (argc < 3)
        {
            printUsage();
            return 1;
        }

        pathBgs  = argv[1];
        pathImgs = argv[2];

        // Used to be more complex parsing back in the days

        return 0;
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    void loadImages(const std::string& ext, const std::string& path, ImgBuffer& imgs, const int& mode)
    {
        std::vector<cv::String> strBuffer;
        cv::glob(cv::String{path} + cv::String{"/*."} + cv::String{ext}, strBuffer, false);

        for (auto& it : strBuffer)
        {
            imgs.push_back(cv::imread(it, mode));
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    void loadImages(const std::string& ext, const std::string& path, ImgBuffer& imgs, StrBuffer& names, const int& mode)
    {
        std::vector<cv::String> strBuffer;
        cv::glob(cv::String{path} + cv::String{"/*."} + cv::String{ext}, strBuffer, false);

        for (auto& it : strBuffer)
        {
            imgs.push_back(cv::imread(it, mode));
            names.push_back(getClassID(it));
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    void getDirectories(const std::string& path, StrBuffer& strBuffer)
    {
        for (auto& p : std::experimental::filesystem::recursive_directory_iterator(path))
        {
            if (p.status().type() == std::experimental::filesystem::file_type::directory)
            {
                strBuffer.push_back(p.path().string());
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    void readAnnotations(const std::string& path, StrBuffer& strBuffer)
    {
        std::ifstream infile{path + "/" + annotation};
        std::string line;

        while (std::getline(infile, line))
        {
            strBuffer.emplace_back(std::move(line));
        }
    }

    ///////////////////////////////////////////////////////////////////////////////////////////////
    void printUsage()
    {
        std::cout << "Usage: <bin_name> <path_2_backgrounds> <path_2_images>" << std::endl;
        std::cout << "e.g. : ./dataset-generator ./backgrounds/ ./data/" << std::endl;
        std::cout << "Note : See Makefile for build flags" << std::endl;
    }
}