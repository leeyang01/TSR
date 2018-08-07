#include "vetmultikcftracker.h"

VetMultiKCFTracker::VetMultiKCFTracker()
{

}

VetMultiKCFTracker::~VetMultiKCFTracker()
{

}


//添加跟踪目标的bounding box
void VetMultiKCFTracker::add_target(const Rect &rect, Mat &img)
{
    KCFTracker tracker;
    //初始化跟踪器
    tracker.init(rect, img);

    //将跟踪的bounding box和跟踪器存入对应的容器
    m_trackerList.push_back(tracker);
    m_trackObjects.push_back(rect);
}


//更新跟踪的目标
void VetMultiKCFTracker::update(Mat &img)
{
    Rect tmp;

    for(int i = 0; i < m_trackerList.size(); i++)
    {
        tmp = m_trackerList.at(i).update(img);
        m_trackObjects.at(i) = tmp;
    }
}

//从跟踪列表中剔除不符合跟踪条件的目标
//剔除目标条件：跟踪框在图像边缘小于一定阈值；连续跟踪达到设定的帧数等
void VetMultiKCFTracker::filter_targets_light(Mat img, vector<int> &consecutive_num)
{
    Mat tem_ROI;
    for(int i = 0; i < m_trackObjects.size(); i++)
    {
        Rect tmp = m_trackObjects.at(i);
        //tem_ROI=img(tmp);
        //int label=VL.blackboxSVM(tem_ROI);
        if((tmp.tl().y<230)||(tmp.tl().x<380) || (tmp.br().x>900)|| (tmp.br().y>430)
                || (tmp.width<8) || (tmp.width>30)||(consecutive_num.at(i)>120) ||(tmp.area()>1400))
        {
            //剔除：目标、对应的跟踪器、统计连续已跟踪的帧数的计数器
//            m_trackObjects.erase(m_trackObjects.begin()+i);
//            m_trackerList.erase(m_trackerList.begin()+i);
//            consecutive_num.erase(consecutive_num.begin()+i);
            m_trackObjects.clear();
            m_trackerList.clear();
            consecutive_num.clear();
        }
    }
}

void VetMultiKCFTracker::filter_targets_warn(Mat img, vector<int> &consecutive_num)
{
    for(int i = 0; i < m_trackObjects.size(); i++)
    {
        Rect tmp = m_trackObjects.at(i);
        if((tmp.tl().y<250)||(tmp.tl().x<480) || (tmp.br().x>1080)|| (tmp.br().y>450)
                || (tmp.width<12) || (tmp.width>55)||(consecutive_num.at(i)>200) ||(tmp.area()>2000))
        {
            //剔除：目标、对应的跟踪器、统计连续已跟踪的帧数的计数器
//            m_trackObjects.erase(m_trackObjects.begin()+i);
//            m_trackerList.erase(m_trackerList.begin()+i);
//            consecutive_num.erase(consecutive_num.begin()+i);
            m_trackObjects.clear();
            m_trackerList.clear();
            consecutive_num.clear();
        }
    }
}
//maskImage函数的功能：把正在处于跟踪状态的目标区域挖掉
void VetMultiKCFTracker::maskImage(Mat &img)
{
    for(int i = 0; i < m_trackObjects.size(); i++)
    {
        Rect rect = m_trackObjects.at(i);
        img(rect).setTo(0);
    }
}
