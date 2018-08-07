#ifndef VETFARROADLIGHTPREPROCESSOR
#define VETFARROADLIGHTPREPROCESSOR
#include<iostream>
#include"cv.h"
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
#define VET_BinaryValue 190
#define VET_LIGHT_X 380//ROI矩形框左上角X坐标值
#define VET_LIGHT_Y 230//ROI矩形框左上角Y坐标值
#define VET_LIGHT_W 500//ROI矩形框宽度
#define VET_LIGHT_H 200//ROI矩形框高度
using namespace std;
using namespace cv;

class vetfarroadlightpreprocessor//交通信号灯预处理类
{
public:
    vetfarroadlightpreprocessor();//构造函数
    ~vetfarroadlightpreprocessor();//析构函数
public:
    void getROIImage(Mat,Mat &);//获取ROI区域函数
    void getMaxGray(Mat,Mat &);//灰度化函数
    void lightPreprocessor(Mat,Mat &);//交通信号灯预处理总流程的封装函数
private:
    void getSmooth(Mat,Mat &);//平滑滤波函数
    void getBinary(Mat,Mat &);//固定阈值二值化
    void eliminateWhiteLight(Mat,Mat,Mat &);//利用RGB三通道颜色方差消除白光影响
    void getHomography(Mat,Mat &);//形态学运算函数
    inline float maxVal(float a,float b);//内联函数，得到两个数的最大值
private:
    string m_filepath;//文件路径
    Mat m_srcImage;//原始图像
    Mat m_roiImage;//roi图像
    Mat m_grayImage;//灰度化图像
    Mat m_smoothImage;//平滑滤波图像
    Mat m_binaryImage;//二值化图像
    Mat m_pureImage;//RGB方差过滤图像
    Mat m_homographyImage;//形态学图像
};
#endif // VETFARROADLIGHTPREPROCESSOR

