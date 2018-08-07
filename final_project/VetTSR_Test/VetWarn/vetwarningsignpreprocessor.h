#ifndef VETWARNINGSIGNPREPROCESSOR
#define VETWARNINGSIGNPREPROCESSOR
#include<iostream>
#include"cv.h"
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#define VET_WARN_X 480//ROI矩形框左上角X坐标
#define VET_WARN_Y 250//ROI矩形框左上角Y坐标
#define VET_WARN_W 600//ROI矩形框宽度
#define VET_WARN_H 200//ROI矩形框高度
using namespace std;
using namespace cv;

class vetwarningsignpreprocessor//警告标志预处理类
{
public:
    vetwarningsignpreprocessor() ;//构造函数
    ~vetwarningsignpreprocessor();//析构函数
public:
    void getROI(Mat,Mat &);//获取ROI图像函数
    void warningProprecessor(Mat,Mat &);//警告标志预处理流程封装函数
private:
    void getEqualizeCorrect(Mat,Mat &);//图像增强，三通道分别均衡化
    void splitYellow(Mat,Mat &);//RGB空间分割黄色，是黄色置为白色，不是黄色置为黑色
private:
    Mat m_srcImage;//原始图像
    Mat m_roiImage;//roi图像
    Mat m_equalizeImage;//均衡化图像
    Mat m_preprocessoredImage;//预处理过后的图像
};
#endif // VETWARNINGSIGNPREPROCESSOR

