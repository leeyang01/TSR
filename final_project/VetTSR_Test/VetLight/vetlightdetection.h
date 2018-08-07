#ifndef VETLIGHTDETECTION
#define VETLIGHTDETECTION
#include "vetlightpreprocessor.h"
#include "vetstatemachine.h"
#include "struct.h"
#include <string>
#include <fstream>
class vetlightdetection{
public:
    vetlightdetection();//构造函数
    ~vetlightdetection();//析构函数
private:
    trafficlight m_Light;
    vector<trafficlight> m_Light_vector;//声明trafficlight类型的容器
    vector<Scalar> m_Scalar;//声明Scalar类型的容器   
public:
    vector<trafficlight> lightDetecte(Mat );// 信号灯公有接口
    int colorRecognise(Mat);//利用RGB空间识别信号灯的颜色
    int blackboxSVM(Mat);// 信号灯识别第3级分类器，判断信号灯的BlackBox
private:
    void findContour(Mat,Mat,vector<vector<Point> > &,vector<trafficlight> &);// 寻找轮廓，并进行面积，宽高比，外接圆半径等条件筛选
    int lightSVM(Mat);// 信号灯识别第一级分类器，判断是否为信号灯，是1，不是-1
    int shapeSVM(Mat);// 信号灯识别第二级分类器，判断信号灯的形状，分为Up,Left,Right以及Circle   
    double BlackBox(Mat);// 统计背板框黑色像素
    void drawImage(Mat,vector<vector<Point> >,Mat &);// 经过筛选过的轮廓，结合第一级分类器判断是否为信号灯，第二级分类器判断信号灯形状，加上颜色空间判断颜色，在ROI图像显示出识别的结果
    void addingROI(Mat,Mat,Mat &);// 经过识别过的ROI图像加载到原图上
    inline int maximum(int a,int b,int c);// 内联函数，取三个参数的最大值
private:
    int m_count;// 输出文件流计数
    int m_lightNum;// 在图像下方信号灯的个数
    int m_showCount;// 在图像下方信号灯显示计数
    int m_upNum;// 统计up number
    int m_leftNum;// 统计left number
    int m_rightNum;// 统计right number
    int m_labelNum;// 信号灯第一级分类器，判断is or not light
    int m_shapeNum;// 信号灯第二级分类器，判断which shape
    int m_colorNum;// 信号灯RGB空间颜色判别which color
    int m_fpsNum;// 视频帧序号
    ofstream m_file;// 文件流
    vector<vector<Point> > m_contours;// 信号灯轮廓
    Mat m_srcImage;// 原图像
    Mat m_roiImage;// ROI图像
    Mat m_preprocessedImage;// 预处理图像
    Mat m_detectedImage;// 检测图像
    Mat m_dstImage;// 显示图像
    bool m_isShow;//是否显示过
    CvSVM m_svm_lightClassifier ;//信号灯第一级分类器
    CvSVM m_svm_shapeClassifier ;//信号灯第二级分类器
    CvSVM m_svm_blackboxClassifier;//信号灯第3级分类器
};
#endif // VETLIGHTDETECTION

