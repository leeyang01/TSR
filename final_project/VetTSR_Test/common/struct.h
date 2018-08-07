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
#define X2 480//ROI���ο����Ͻ�X����
#define Y2 250//ROI���ο����Ͻ�Y����
#define W2 600//ROI���ο���
#define H2 200//ROI���ο�߶�
using namespace cv;
using namespace std;

typedef pair<timeb, Mat> PAIR_T_M;
typedef list<pair<timeb, Mat> > LIST_P;

struct  CapturerInfo
{
	bool _state;  //�Ƿ��
	int _open_mode; //�򿪷�ʽ
	int _camera_num;  //����ͷ���
	string _open_info;   //������Ϣ(��Ƶ����/����ͷ���)
};

enum Result {
	turn=0, //��ֹ��ת
	right=1, //��ֹ��ת
	in=2, //��ֹͨ��
	speed_40=3, 
	speed_50=4, 
	speed_60=5, 
	onevehicle=6, //��ֹ���ֳ���ʻ��
	honk=7, //��������
	goods=8, //��ֹ�ػ�����ʻ��
	bus=9, //��ֹ���Ϳͳ�ʻ��
	park=10, //��ֹͣ��
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
struct trafficlight{//����һ���źŵƼ����ʶ���������ṹ��
    int m_fps;//��⵽���źŵ�ͼ��֡���
    Point m_point;//��⵽���źŵ����ĵ�����
    CvRect m_Rect;//��⵽���źŵ�Rect
    double m_area;//��⵽���źŵƵ����
    Scalar m_color;//��⵽���źŵƵ���ɫ
    string m_shape;//��⵽���źŵƵ���״
    string m_direction;//��⵽���źŵƵİڷ�λ�ã���ֱ/ˮƽ)
    string m_meaning;//
};

struct warn{// ���徯���־����ṹ��
    int m_label;// label
    int m_warnFPS;// ֡��
    Point m_warnPoint;// ���ĵ�
    Rect m_warnRect;// ���ο�
    string m_warnMeaning;// �����־����
};
#endif // STRUCTCAPTURERINFO

