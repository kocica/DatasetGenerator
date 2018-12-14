/**
 * @file    GeneratorTransparent.hpp
 * @author  Filip Kocica <xkocic01@stud.fit.vutbr.cz>
 * @date    05/10/2018
 *
 * Training dataset generator
 *
 * This file provides generating of training images to backgrounds
 */

#pragma once

// STD/STL
#include <sstream>

// Local
#include "Generator.hpp"

/** @brief Alias for buffer of strings */
using StrBuffer_t = std::vector<std::string>;

/**
 * @brief Format of TS annotations
 *        Filename;Width;Height;Roi.X1;Roi.Y1;Roi.X2;Roi.Y2;ClassId
 */
enum AnnotationFormat_t
{
	NAME   = 0,
	WIDTH  = 1,
	HEIGHT = 2,
	RX1    = 3,
	RY1    = 4,
	RX2    = 5,
	RY2    = 6,
	CLASS  = 7
};

/**
 * @brief Structure representing annotation of TS
 */
struct Annotation_t
{
    std::string name;
    int w;
    int h;
    int rx1;
    int ry1;
    int rx2;
    int ry2;
    std::string className;
};

/**
 * @brief Class providing generating of images and its annotation using cropped TS's
 */
class DatasetGeneratorCropped_t : public DatasetGenerator_t
{

public:

    DatasetGeneratorCropped_t() = delete;
    explicit DatasetGeneratorCropped_t(std::ofstream& out, int imgClass, const StrBuffer_t& tsAnnots, const std::string& classID);
    virtual ~DatasetGeneratorCropped_t() = default;

    /************************************************************************************
     * @brief Generating of images & annotation (Image of traffic sign with alpha channel)
     *
     * @param b [in] buffer of ROIs
     * @param m [in] background
     * @param m2 [in] image to be copied to background
     */
#ifdef ROI_SELECTION
	virtual void generateDataset(const ROIBuffer_t& b, cv::Mat m, cv::Mat m2) override;
#else
	virtual void generateDataset(cv::Mat m, cv::Mat m2) override;
#endif

protected:

    /************************************************************************************
     * @brief Find approtimate annotation and parse it
     */
    void parseAnnotations(Annotation_t& annot) const;

    /************************************************************************************
     * @brief Parse CSV line to single values in vector
     *
     * @param s [in] Annotation CSV line
     * @param delim [in] Delimeter between values - most likely ',' or ';'
     * @param elems [out] Parsed elements of CSV
     */
    void splitAnnotation(const std::string& s, const char& delim, StrBuffer_t& elems) const;

private:
    /** @brief Annotations of cropped TS's */
    StrBuffer_t m_tsAnnotations;

    /** @brief ID of class */
    std::string m_classID;

};