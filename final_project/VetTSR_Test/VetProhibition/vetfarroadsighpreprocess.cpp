
#include"vetfarroadsighpreprocess.h"
#include<fstream>
#include<iostream>
using namespace std;
ofstream outfile("D:/red.csv",ios::app);
VetFarRoadSignPreprocessor::VetFarRoadSignPreprocessor()
{
}

VetFarRoadSignPreprocessor::~VetFarRoadSignPreprocessor()
{
}

Mat VetFarRoadSignPreprocessor::getCurrentPreprocessedImage(Mat & initialData){
    //_image_data = initialData.second;
    rectImage(initialData);
	//gammaCorrection(_image_data);
    gaussianblur(initialData);
    binaryImage(initialData);
    erodeImage(initialData);
    dilateImage(initialData);
    return initialData;
}

void VetFarRoadSignPreprocessor::rectImage(Mat& imagedata){
	Mat m_temp;
	resize(imagedata, m_temp, Size(imagedata.cols / 2, imagedata.rows / 2));
	Mat m_data(m_temp, Rect(m_temp.cols*1/3, 0, m_temp.cols*2/3, m_temp.rows*5/8));
   // Mat kernel=(Mat_<float>(3,3)<<0,-1,0,-1,5,-1,0,-1,0);
 //   filter2D(m_data,m_data,CV_8UC3,kernel);
	imagedata = m_data.clone();
}

Mat VetFarRoadSignPreprocessor::norm_0_255(const Mat& src) {
	// Create and return normalized image:  
	Mat dst;
	switch (src.channels()) {
	case 1:
		cv::normalize(src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
		break;
	case 3:
		cv::normalize(src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
		break;
	default:
		src.copyTo(dst);
		break;
	}
	return dst;
}
void VetFarRoadSignPreprocessor::gammaCorrection(Mat& imagedata){
	cvtColor(imagedata,imagedata,CV_BGR2HSV);
	vector<Mat> m;
	split(imagedata,m);
	float gamma = 0.1;
	Mat x, o;
	m[2].convertTo(x, CV_32FC1);
	pow(x, gamma, o);
	m[2] = norm_0_255(o);
	merge(m, imagedata);
	cvtColor(imagedata, imagedata, CV_HSV2BGR);
	imshow("t", imagedata);
}
void VetFarRoadSignPreprocessor::binaryImage(Mat& imagedata){
    Mat img(imagedata.rows, imagedata.cols, CV_8UC1, Scalar(0));
        float r, g, b;
        int nr = imagedata.rows;
        int nc = imagedata.cols;
        if (imagedata.isContinuous())
        {
            nr = 1;
            nc = nc*imagedata.rows*imagedata.channels();
        }
        for (int i = 0; i<nr; i++)
        {
            const uchar* inData = imagedata.ptr<uchar>(i);
            uchar* outData = img.ptr<uchar>(i);
            for (int j = 0; j<nc; j += imagedata.channels())
            {
                r = (float)*(inData+2) / (float)(*(inData + 1) + *inData + *(inData+2));
                g = (float)*(inData + 1) / (float)(*(inData + 1) + *inData + *(inData + 2));
                b = (float)*(inData) / (float)(*(inData + 1) + *inData + *(inData + 2));
                if (((r - g) > 0.07 && (r - b)>0.07 && (g - b) < 0.12&&r / g > 1.4)){//红色
                    *outData = 255;
                }
                inData++;
                inData++;
                inData++;
                outData++;
            }
        }
        imagedata = img.clone();

}

void VetFarRoadSignPreprocessor::gaussianblur(Mat& imagedata){
	GaussianBlur(imagedata, imagedata, Size(1, 1), 0, 0);
}

void VetFarRoadSignPreprocessor::erodeImage(Mat& imagedata){
	Mat mat_temp = getStructuringElement(MORPH_RECT, Size(1, 1));
	erode(imagedata, imagedata, mat_temp);//图像腐蚀
}

void VetFarRoadSignPreprocessor::dilateImage(Mat& imagedata){
	Mat mat_temp1 = getStructuringElement(MORPH_RECT, Size(4, 4));
	dilate(imagedata, imagedata, mat_temp1);//图像膨胀,先腐蚀后膨胀（开运算，消除小物体，分离物体）可以去掉目标的孤立点
}
