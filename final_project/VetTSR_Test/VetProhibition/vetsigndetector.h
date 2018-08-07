#ifndef VETSIGNDETECTOR_H
#define VETSIGNDETECTOR_H
//#include "stdafx.h"
#include"vetfarroadsighpreprocess.h"
#include"struct.h"

using namespace cv;
using namespace std;
class VetSignDetector
{
public:
    VetSignDetector();
    ~VetSignDetector();

public:
    vector<RectRegion> signDetector(Mat&,Mat&);

private:
    void getSignCircle(Mat&,Mat&);  //标志牌候选区域获取
    double round(double);  //四舍五入
    void recogSign(Mat&);  //标志牌候选区域识别
    float recogPicture(Mat&);  //使用SVM模型进行分类
    int detectNumber(Mat&);
    int getGrayThre(Mat);
    void finddigitalContours(Mat&,Mat&);
    int recogNumber(Mat&);
public:
    vector<CvRect> rect_region_sign;
    vector<CvRect> sign_speed;
private:
    vector<CvRect> rect_region_number;

    vector<RectRegion> rect_region;

    int i_num_region_sign;
    CvSVM svm_circle;
    CvSVM svm_red_first;
    CvSVM svm_red_second;
    CvSVM svm_digital;
};

#endif //VETSIGNDETECTOR_H
