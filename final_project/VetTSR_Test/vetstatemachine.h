/**
* [
*  Copyright (c) 2016 by Vehicle-Eye Technology
*  ALL RIGHTS RESERVED.
*
*  The software and information contained herein are proprietary to,  and comprise
*  valuable trade secrets of, Vehicle-Eye Technology.  Disclosure of the software
*  and information will materially harm the business of Vehicle-Eye Technology.
*  This software is furnished pursuant to a written development agreement and may
*  be used, copied, transmitted, and stored only in accordance with the terms of
*  such agreement and with the inclusion of the above copyright notice.  This
*  software and information or any other copies thereof may not be provided or
*  otherwise made available to any other party.
* ]
*/

/*!
* \file vetobjectmanager.h
* \author [Zeyu Zhang]
* \version [0.1]
* \date 2017-04-26
*/

#ifndef VETOBJECTMANAGER_H
#define VETOBJECTMANAGER_H
//#include "vetroi.h"
//#include "vetutils.h"
#include <opencv2/opencv.hpp>

#include <iostream>

typedef struct _lightAttr
{
    cv::vector <vector<Point> > _contours;
//    cv::Rect rect_;
//    std::string _color;
//    std::string _shape;
//    int _area;

}lightAttr;
typedef struct _DetectedRegion
{
    cv::vector <vector<Point> > contours_;
//    cv::Rect rect_;
//    std::string color_;
//    std::string shape_;
//    int area_;
    int ack_;
    int unack_;
    bool is_exist_;
}DetectedRegion;

class VetObjectManager
{
public:
    VetObjectManager(double overlap_threshold = 5, int ack_threshold = 2,
        int unack_threshold = 1, int delete_threshold = 2);
    ~VetObjectManager();

public:
    void update(std::vector<lightAttr> &detected_res);
    void _getDetectedRegion(std::vector<lightAttr> &detected_res);

private:
    double _calcOverlapped(const cv::Rect &a, const cv::Rect &b);
    double _calcDistance(const cv::vector <vector<Point> > & a,const cv::vector <vector<Point> > & b);
    void _updateDetectedRegion(DetectedRegion &dst, const lightAttr &src);
    void _addDetectedRegion(const lightAttr &src);
    void _clearNotExistRegion();

private:
    std::vector<DetectedRegion> detected_regions_;

    // determine if two regions matched each other
    double overlap_threshold_;

    // only when ack_ >= ack_threshold_, region is sound
    int ack_threshold_;

    // only when unack_ <= unack_threshold_, region is sound
    int unack_threshold_;

    // when unack_ >= delete_threshold_, region is removed from
    // the vector detected_regions_
    int delete_threshold_;
    unsigned char id_;
};

#endif // VETOBJECTMANAGER_H
