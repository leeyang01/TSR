#ifndef STRUCTCAPTURERINFO
#define STRUCTCAPTURERINFO

#include <iostream>
#include <fstream>
#include <ctime>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/ml/ml.hpp>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <cv.h>
#include<opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <list>
#include <sys/timeb.h>
#define X2 480//ROI矩形框左上角X坐标
#define Y2 250//ROI矩形框左上角Y坐标
#define W2 600//ROI矩形框宽度
#define H2 200//ROI矩形框高度
using namespace cv;
using namespace std;

typedef pair<timeb, Mat> PAIR_T_M;
typedef list<pair<timeb, Mat> > LIST_P;

struct  CapturerInfo
{
	bool _state;  //是否打开
	int _open_mode; //打开方式
	int _camera_num;  //摄像头编号
	string _open_info;   //具体信息(视频名称/摄像头编号)
};

enum Result {
	turn=0, //禁止回转
	right=1, //禁止右转
	in=2, //禁止通行
	speed_40=3, 
	speed_50=4, 
	speed_60=5, 
	onevehicle=6, //禁止该种车型驶入
	honk=7, //禁鸣喇叭
	goods=8, //禁止载货车辆驶入
	bus=9, //禁止大型客车驶入
	park=10, //禁止停车
	light=11, 
	null=12 
};

struct RectRegion
{
	int _start_point_x;
	int _start_point_y;
	int _width;
	int _height;
	Result _traffic_sign;
};
struct trafficlight{//定义一个信号灯检测与识别输出结果结构体
    int m_fps;//检测到的信号灯图像帧编号
    Point m_point;//检测到的信号灯中心点坐标
    CvRect m_Rect;//检测到的信号灯Rect
    double m_area;//检测到的信号灯的面积
    Scalar m_color;//检测到的信号灯的颜色
    string m_shape;//检测到的信号灯的形状
    string m_direction;//检测到的信号灯的摆放位置（竖直/水平)
    string m_meaning;//
};

struct warn{// 定义警告标志输出结构体
    int m_label;// label
    int m_warnFPS;// 帧号
    Point m_warnPoint;// 中心点
    Rect m_warnRect;// 矩形框
    string m_warnMeaning;// 警告标志语义
};
#endif // STRUCTCAPTURERINFO

