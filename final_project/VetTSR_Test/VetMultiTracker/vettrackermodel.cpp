#include"vettrackermodel.h"

VettrackerModel::VettrackerModel(){
    m=0;
    //
}
VettrackerModel::~VettrackerModel(){
    //
}
// traffic light tracker
void VettrackerModel::TrafficLight(Mat srcImage_, vector<trafficlight>& light_vector_)
{
    Mat tmp_img=srcImage_.clone();
    m_light_vector=Vl.lightDetecte(tmp_img);
    for(int i=0;i<m_light_vector.size();i++){
        m_multi_tracker_light.add_target(m_light_vector[i].m_Rect, tmp_img);
        int flag = 0;
        m_consecutive_num_light.push_back(flag);
    }
    //cout<<"m_trackObjects.size"<<m_multi_tracker_light.m_trackObjects.size()<<endl;
    if(m_multi_tracker_light.m_trackObjects.size() > 0)// has track target
    {
        //cout << "track_objects:" << m_multi_tracker.m_trackObjects.size() << endl;
        m_multi_tracker_light.update(tmp_img);
        //统计跟踪帧数
        for(int i = 0; i < m_consecutive_num_light.size(); i++)
        {
            ++m_consecutive_num_light.at(i);
            //cout << "consecutive_num:" << m_consecutive_num.at(i) << endl;
        }
        for(int k1 = 0; k1 < m_multi_tracker_light.m_trackObjects.size()-1; k1++)
        {
            for(int k2 = k1+1; k2 < m_multi_tracker_light.m_trackObjects.size(); k2++)
            {
                double overlap_rate = calcOverlapped(m_multi_tracker_light.m_trackObjects.at(k1),m_multi_tracker_light.m_trackObjects.at(k2));
                //cout << "overlap_rate:" << overlap_rate << endl;
                if(overlap_rate > 0)
                {
                    m_multi_tracker_light.m_trackObjects.erase(m_multi_tracker_light.m_trackObjects.begin()+k2);
                    m_multi_tracker_light.m_trackerList.erase(m_multi_tracker_light.m_trackerList.begin()+k2);
                    m_consecutive_num_light.erase(m_consecutive_num_light.begin()+k2);
                }
            }
        }
        m_multi_tracker_light.filter_targets_light(tmp_img, m_consecutive_num_light);//将特定条件的跟踪目标剔除
        m_multi_tracker_light.maskImage(tmp_img);
    }//将正在跟踪的区域挖掉，不要在此处进行检测（同时，这时检测到的目标一定是需要加入跟踪的目标）

    for(int j = 0; j < m_multi_tracker_light.m_trackObjects.size(); j++)
    {
        Rect tmp = m_multi_tracker_light.m_trackObjects.at(j);
        // rectangle(srcImage_, tmp, Scalar(0,255,0),1,8);
        trafficlight tmp_traffic;
        tmp_traffic.m_Rect = tmp;
        Mat out1=srcImage_(tmp);
        int color=Vl.colorRecognise(out1);
        switch (color) {
        case 1:
            tmp_traffic.m_color=Scalar(0,0,255);
            tmp_traffic.m_meaning="Stop";
            break;
        case 2:
            tmp_traffic.m_color=Scalar(0,255,0);
            tmp_traffic.m_meaning="Go";
            break;
        case 3:
            tmp_traffic.m_color=Scalar(0,255,255);
            tmp_traffic.m_meaning="Warn";
            break;
        default:
            break;
        }
        light_vector_.push_back(tmp_traffic);
    }
    for(int i=0;i<light_vector_.size();i++)
    {
        for(int j=light_vector_.size()-1;j>=1;j--)
        {
            if(light_vector_[j-1].m_Rect.x<light_vector_[j].m_Rect.x)
            {
                trafficlight temp=light_vector_[j-1];
                light_vector_[j-1]=light_vector_[j];
                light_vector_[j]=temp;
            }
        }
    }
}

//warn sign tracker
void VettrackerModel::WarnSign(Mat srcImage_, vector<warn>& warn_vector_ )
{
    //vector<Mat> frame;
    Mat tmp_img=srcImage_.clone();
    m_warn_vector=VM.warningdetector(tmp_img);
    for(int i=0;i<m_warn_vector.size();i++)
    {
        m_multi_tracker_warn.add_target(m_warn_vector[i].m_warnRect, tmp_img);
        int flag = 0;
        m_consecutive_num_warn.push_back(flag);
    }
    if(m_multi_tracker_warn.m_trackObjects.size() > 0)// has track target
    {
        //cout << "track_objects:" << m_multi_tracker.m_trackObjects.size() << endl;
        m_multi_tracker_warn.update(tmp_img);

        //统计跟踪帧数
        for(int i = 0; i < m_consecutive_num_warn.size(); i++)
        {
            ++m_consecutive_num_warn.at(i);
            //cout << "consecutive_num:" << m_consecutive_num.at(i) << endl;
        }
        for(int k1 = 0; k1 < m_multi_tracker_warn.m_trackObjects.size()-1; k1++)
        {
            for(int k2 = k1+1; k2 < m_multi_tracker_warn.m_trackObjects.size(); k2++)
            {
                double overlap_rate = calcOverlapped(m_multi_tracker_warn.m_trackObjects.at(k1),m_multi_tracker_warn.m_trackObjects.at(k2));
                //cout << "overlap_rate:" << overlap_rate << endl;
                if(overlap_rate > 0)
                {
                    m_multi_tracker_warn.m_trackObjects.erase(m_multi_tracker_warn.m_trackObjects.begin()+k2);
                    m_multi_tracker_warn.m_trackerList.erase(m_multi_tracker_warn.m_trackerList.begin()+k2);
                    m_consecutive_num_warn.erase(m_consecutive_num_warn.begin()+k2);
                }
            }
        }
    }
        m_multi_tracker_warn.filter_targets_warn(tmp_img, m_consecutive_num_warn);//将特定条件的跟踪目标剔除
        m_multi_tracker_warn.maskImage(tmp_img);//将正在跟踪的区域挖掉，不要在此处进行检测（同时，这时检测到的目标一定是需要加入跟踪的目标）
        for(int j = 0; j < m_multi_tracker_warn.m_trackObjects.size(); j++)
        {
            Rect tmp = m_multi_tracker_warn.m_trackObjects.at(j);
            // rectangle(srcImage_, tmp, Scalar(0,255,0),1,8);
            warn tmp_warn;
            tmp_warn.m_warnRect = tmp;
            Mat warnImage=srcImage_(tmp);
            //frame.push_back(warnImage);
            int label=VM.warningShape(warnImage);
            switch (label)
            {
            case 1:
                tmp_warn.m_warnMeaning="SlowDown";
                tmp_warn.m_label=1;
                break;
            case 2:
                tmp_warn.m_label=2;
                tmp_warn.m_warnMeaning="People";
                break;
            case 3:
                tmp_warn.m_label=3;
                tmp_warn.m_warnMeaning="Child";
                break;
            case 4:
                tmp_warn.m_label=4;
                tmp_warn.m_warnMeaning="Construction";
                break;
            default:
                tmp_warn.m_label=-1;
                break;
            }
            if( tmp_warn.m_label != -1)
            {
            warn_vector_.push_back(tmp_warn);
            }
        }
//        for(int i=0;i<frame.size();i++){
//            char name[200];
//            sprintf(name, "%s%.6d%s", "/media/lee/Lee-Disk/TSR文件夹/警告标志检测与识别/Data/警告标志训练集/第一级分类器（识别是否为警告标志）/warn frame/", ++m, ".jpg");//保存的图片名
//            imwrite(name,frame[i]);
//        }
}

//计算两个bounding box 的重叠率
double VettrackerModel::calcOverlapped( const Rect &r1, const Rect &r2)
{
    //查找重叠区域bounding box 的左上角（top_left）和右下角（bottom_right）的坐标
    int overlap_x1 = max(r1.tl().x, r2.tl().x);
    int overlap_y1 = max(r1.tl().y, r2.tl().y);
    int overlap_x2 = min(r1.br().x, r2.br().x);
    int overlap_y2 = min(r1.br().y, r2.br().y);
    //计算相交面积
    int w = max(0, overlap_x2-overlap_x1);
    int h = max(0, overlap_y2-overlap_y1);
    double interArea = w * h;
    //这里计算的是交集与最小的bounding box的面积比值
    double rate = interArea / (double)(r1.area()+r2.area()-interArea);
    return rate;
}
