#ifndef VETMULTIKCFTRACKER_H
#define VETMULTIKCFTRACKER_H

#include "vetkcftracker.hpp"
#include "opencv2/opencv.hpp"
#include"vetlightdetection.h"
#include <iostream>

using namespace cv;
using namespace std;

class VetMultiKCFTracker
{
public:
    VetMultiKCFTracker();
    ~VetMultiKCFTracker();

public:
    void add_target(const Rect &rect, Mat &img);
    void update(Mat &img);
    void filter_targets_light(Mat img, vector<int> &consecutive_num);
    void filter_targets_warn(Mat img, vector<int> &consecutive_num);
    void maskImage(Mat &img);//将图像中在跟踪的区域挖掉


public:
    vector<KCFTracker> m_trackerList;
    vector<Rect> m_trackObjects;
    vetlightdetection VL;
//    vector<int> _consecutive_num;



};

#endif // VETMULTIKCFTRACKER_H
