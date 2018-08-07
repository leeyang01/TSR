#ifndef VETMODELSTORE_H
#define VETMODELSTORE_H

#include "vetlightpreprocessor.h"
#include "vetkcftracker.hpp"
#include "vetmultikcftracker.h"
#include"struct.h"
#include"vetlightdetection.h"
#include"vetwarningsigndetection.h"
#include"vetsigndetector.h"

#include <iostream>
#include <fstream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>

class VettrackerModel
{
public:
    VettrackerModel();
    ~VettrackerModel();

public:
    void TrafficLight(Mat,vector<trafficlight> &);
    void WarnSign(Mat,vector<warn> &);
private:
    int m;
    double calcOverlapped( const Rect &r1, const Rect &r2);// 计算两个bounding box 的重叠率
    KCFTracker tracker;
    bool track_flag = false;
    bool detect_flag = true;
    vetlightdetection Vl;
    vetwarningsigndetector VM;
    vector<int> m_consecutive_num_light;
    VetMultiKCFTracker m_multi_tracker_light;
    vector<int> m_consecutive_num_warn;
    VetMultiKCFTracker m_multi_tracker_warn;
    vector<trafficlight> m_light_vector;
    vector<warn> m_warn_vector;
  };

#endif // VETMODELSTORE_H
