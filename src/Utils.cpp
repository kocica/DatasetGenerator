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
    int parseArgs(int argc, char **argv, std::string& pathBgs, std::string& pathImgs, cv::Size& s)
    {
        if (argc < 5)
        {
            printUsage();
            return 1;
        }

        pathBgs  = argv[1];
        pathImgs = argv[2];
        s        = cv::Size{ std::stoi(argv[3]), std::stoi(argv[4]) };

        return 0;
    }

    void loadImages(const std::string& path, ImgBuffer& imgs, const int& mode)
    {
        std::vector<cv::String> strBuffer;
        cv::glob(path, strBuffer, false);

        for (auto& it : strBuffer)
        {
            imgs.push_back(cv::imread(it, mode));
        }
    }

    StrBuffer getDirectories(const std::string& path)
    {
        StrBuffer r;
        
        /*
        for(auto& p : std::filesystem::recursive_directory_iterator(path))
        {
            if(p.status().type() == std::filesystem::file_type::directory)
            {
                r.push_back(p.path().string());
            }
        }
        */

        return r;
    }

   void printUsage()
    {
        std::cout << "Usage: dataset_generator TODO :-)" << std::endl;
    }
}