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
    void getSignCircle(Mat&,Mat&);  //��־�ƺ�ѡ�����ȡ
    double round(double);  //��������
    void recogSign(Mat&);  //��־�ƺ�ѡ����ʶ��
    float recogPicture(Mat&);  //ʹ��SVMģ�ͽ��з���
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
