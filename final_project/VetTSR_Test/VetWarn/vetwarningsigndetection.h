#ifndef VETWARNINGSIGNDETECTION
#define VETWARNINGSIGNDETECTION
#include"struct.h"
#include<iostream>
#include"cv.h"
#include<opencv2/imgproc/imgproc.hpp>
#include<opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;
class vetwarningsigndetector// 警告标志检测类
{
public:
    vetwarningsigndetector() ;// 构造函数
    ~vetwarningsigndetector();// 析构函数
private:
    warn m_Warn;
    vector<warn> m_Warn_vector;// 定义警告标志结构体容器
public:
    vector<warn> warningdetector(Mat );// 警告标志公有接口
    int warningShape(Mat);// 第二级分类器，判断警告标志具体含义
private:
    void findContour(Mat,Mat,vector<vector<Point> > &);// 寻找轮廓，并进行面积，宽高比，外接圆半径等条件筛选
    int warningSVM(Mat);// 第一级分类器，判断是否为警告标志 
    void drawImage(Mat,vector<vector<Point> >,vector<warn> &,Mat &);// 经过筛选过的轮廓，结合第一级分类器判断是否为警告标志，第二级分类器判断警告标志具体含义，在ROI图像显示出识别的结果
    void addingROI(Mat,Mat,Mat &);// 经过识别过的ROI图像加载到原图上

private:
    int m;// the order of output image
    int m_fpsNum;// 图像帧编号
    double m_fps;// 帧率
    vector<vector<Point> > m_contours;// 警告标志轮廓
    Mat m_srcImage;// 原图
    Mat m_roiImage;// ROI图
    Mat m_preprocessoredImage;// 预处理图像
    Mat m_detectedImage;// 检测与识别后的ROI图像
    Mat m_dstImage;// 显示图像
    Mat m_people;// 注意行人
    Mat m_child;// 注意儿童
    Mat m_slowdown;// 减速慢行
    Mat m_slowdown_Mask;// 注意行人掩图
    Mat m_people_Mask;// 注意儿童掩图
    Mat m_child_Mask;// 减速慢行掩图
    CvSVM m_svm_warningClassifier ;//第一级分类器
    CvSVM m_svm_shapeClassifier ;//第二级分类器
};
#endif // VETWARNINGSIGNDETECTION

