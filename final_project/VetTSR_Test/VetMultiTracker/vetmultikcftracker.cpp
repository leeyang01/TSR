#include "vetmultikcftracker.h"

VetMultiKCFTracker::VetMultiKCFTracker()
{

}

VetMultiKCFTracker::~VetMultiKCFTracker()
{

}


//��Ӹ���Ŀ���bounding box
void VetMultiKCFTracker::add_target(const Rect &rect, Mat &img)
{
    KCFTracker tracker;
    //��ʼ��������
    tracker.init(rect, img);

    //�����ٵ�bounding box�͸����������Ӧ������
    m_trackerList.push_back(tracker);
    m_trackObjects.push_back(rect);
}


//���¸��ٵ�Ŀ��
void VetMultiKCFTracker::update(Mat &img)
{
    Rect tmp;

    for(int i = 0; i < m_trackerList.size(); i++)
    {
        tmp = m_trackerList.at(i).update(img);
        m_trackObjects.at(i) = tmp;
    }
}

//�Ӹ����б����޳������ϸ���������Ŀ��
//�޳�Ŀ�����������ٿ���ͼ���ԵС��һ����ֵ���������ٴﵽ�趨��֡����
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
            //�޳���Ŀ�ꡢ��Ӧ�ĸ�������ͳ�������Ѹ��ٵ�֡���ļ�����
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
            //�޳���Ŀ�ꡢ��Ӧ�ĸ�������ͳ�������Ѹ��ٵ�֡���ļ�����
//            m_trackObjects.erase(m_trackObjects.begin()+i);
//            m_trackerList.erase(m_trackerList.begin()+i);
//            consecutive_num.erase(consecutive_num.begin()+i);
            m_trackObjects.clear();
            m_trackerList.clear();
            consecutive_num.clear();
        }
    }
}
//maskImage�����Ĺ��ܣ������ڴ��ڸ���״̬��Ŀ�������ڵ�
void VetMultiKCFTracker::maskImage(Mat &img)
{
    for(int i = 0; i < m_trackObjects.size(); i++)
    {
        Rect rect = m_trackObjects.at(i);
        img(rect).setTo(0);
    }
}
