
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
* \file VetObjectManager.cpp
* \author [Zeyu Zhang]
* \version [0.1]
* \date 2017-04-26
*/

#include "vetstatemachine.h"

#include <opencv2/opencv.hpp>

#include <iostream>

using namespace std;
using namespace cv;

VetObjectManager::VetObjectManager(double overlap_threshold, int ack_threshold,
    int unack_threshold, int delete_threshold)
{
    this->overlap_threshold_ = overlap_threshold;
    this->ack_threshold_ = ack_threshold;
    this->unack_threshold_ = unack_threshold;
    this->delete_threshold_ = delete_threshold;
    id_ = 0;
}

VetObjectManager::~VetObjectManager()
{
    cout << "[VetObjectManager::~VetObjectManager] delete VetObjectManager" << endl;
}

void VetObjectManager::update(vector<lightAttr> &detected_res)
{
//    cout << "before NMS: " << detected_res.size() << endl;
    //NMS(detected_res,0.3);
//    cout << "after NMS: " << detected_res.size() << endl;
    for(vector<lightAttr>::iterator iter_res = detected_res.begin(); iter_res != detected_res.end(); )
    {
        vector<DetectedRegion>::iterator iter_regions = detected_regions_.begin();
        while(iter_regions != detected_regions_.end())
        {
            double overlap_rate = _calcDistance(iter_regions->_contours, iter_res->_contours);

            if(overlap_rate > overlap_threshold_)
            {
                _updateDetectedRegion(*iter_regions, *iter_res);
                break;
            }

            iter_regions++;
        }

        // did not find threshold-satisfied overlapped region
        if( iter_regions == detected_regions_.end() )
        {
            _addDetectedRegion(*iter_res);
        }

        iter_res = detected_res.erase(iter_res);
    }

    _clearNotExistRegion();
    _getDetectedRegion(detected_res);
    // _clearNotExistRegion();
}

void VetObjectManager::_getDetectedRegion(vector<lightAttr> &detected_res)
{
    detected_res.clear();

    vector<DetectedRegion>::iterator iter_regions = detected_regions_.begin();

    while(iter_regions != detected_regions_.end())
    {
        if(iter_regions->ack_ >= ack_threshold_ && iter_regions->unack_ <= unack_threshold_)
        {
            //cv::vector <vector<Point> > contours_;
//            cv::Rect rect_;
//            std::string color_;
//            std::string shape_;
//            int area_;
            lightAttr res{iter_regions->contours_};
            detected_res.push_back(res);
        }

        iter_regions++;
    }
}

double VetObjectManager::_calcOverlapped(const Rect &a, const Rect &b)
{
    // find the overlap bounding box top-left & bottom-right coordinates
    int overlap_x1 = max(a.tl().x, b.tl().x);
    int overlap_y1 = max(a.tl().y, b.tl().y);
    int overlap_x2 = min(a.br().x, b.br().x);
    int overlap_y2 = min(a.br().y, b.br().y);

    // compute the width and height of the overlap between
    // computed bounding box(cur_rect) and the bounding box(iter)
    int overlap_width = max(0, overlap_x2 - overlap_x1 + 1);
    int overlap_height = max(0, overlap_y2 - overlap_y1 + 1);

    // compute the maximum ratio of overlap between the area a and area b
    double overlap_area = (float)(overlap_width * overlap_height);
    double overlap_rate = max(overlap_area / a.area(), overlap_area / b.area());

    return overlap_rate;
}

double VetObjectManager::_calcDistance(const cv::vector<vector<Point> > &a, const cv::vector<vector<Point> > &b){
    Point2f center_a; float radius_a;
    minEnclosingCircle(a, center_a, radius_a);
    Point2f center_b; float radius_b;
    minEnclosingCircle(b, center_b, radius_b);
    double distance;
    distance = powf((center_a.x - center_b.x),2) + powf((center_a.y - center_b.y),2);
    distance = sqrtf(distance);
    return distance;
}
void VetObjectManager::_updateDetectedRegion(DetectedRegion &dst, const lightAttr &src) //id不应该有变化
{
    dst.contours_ = src._contours;
//    dst.color_ = src._color;
//    dst.shape_ = src._shape;
//    dst.area_ = src._area;
    dst.ack_++;
    dst.unack_ = 0;
    dst.is_exist_ = true;
}

void VetObjectManager::_addDetectedRegion(const lightAttr &src)
{
    DetectedRegion dst;
    //dst.time_ = src;
    dst.contours_=src._contours;
//    dst.rect_ = src.rect_;
//    dst.color_ = src._color;
//    dst.shape_ = src._shape;
//    dst.area_ = src._area;
    dst.ack_ = 1;
    dst.unack_ = 0;
    dst.is_exist_ = true;

    detected_regions_.push_back(dst);
}

void VetObjectManager::_clearNotExistRegion()
{
    vector<DetectedRegion>::iterator iter_regions = detected_regions_.begin();

    while(iter_regions != detected_regions_.end())
    {
        if(iter_regions->is_exist_ == false)
        {
            iter_regions->ack_--;
            iter_regions->unack_++;
        }
        else
        {
            iter_regions->is_exist_ = false;
        }

        if(iter_regions->unack_ >= delete_threshold_)
        {
            iter_regions = detected_regions_.erase(iter_regions);
        }
        else
        {
            iter_regions++;
        }
    }
}
