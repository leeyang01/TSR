#include"vetlightpreprocessor.h"
vetfarroadlightpreprocessor::vetfarroadlightpreprocessor()
{
	//默认构造函数
}
vetfarroadlightpreprocessor::~vetfarroadlightpreprocessor()
{
	//默认析构函数
}
inline float vetfarroadlightpreprocessor::maxVal(float a,float b)//内联函数，得到两个数的最大值
{
    if(a>b){
        return a;
    }else{
        return b;
    }
}
//信号灯预处理接口
void vetfarroadlightpreprocessor::lightPreprocessor(Mat roiImage_,Mat &homographyImage_)
{
    m_roiImage=roiImage_.clone();
    getMaxGray(m_roiImage,m_grayImage);
    getSmooth(m_grayImage,m_smoothImage);
    getBinary(m_smoothImage,m_binaryImage);
    eliminateWhiteLight(m_roiImage,m_binaryImage,m_pureImage);
    getHomography(m_pureImage,m_homographyImage);
    homographyImage_=m_homographyImage.clone();
}
//截取原图的ROI区域
void vetfarroadlightpreprocessor::getROIImage(Mat srcImage_,Mat& roiImage_)
{
    Rect rect=Rect(VET_LIGHT_X,VET_LIGHT_Y,VET_LIGHT_W,VET_LIGHT_H);
    roiImage_=srcImage_(rect);
}

//R和G通道最大值灰度化
void vetfarroadlightpreprocessor::getMaxGray(Mat roiImage_,Mat& grayImage_)
{
        grayImage_.create(roiImage_.size(),CV_8UC1);
        float r,g;
        for(int i=0;i<roiImage_.rows;i++)
        {
            uchar* data=grayImage_.ptr<uchar>(i);
            for(int j=0;j<roiImage_.cols;j++)
            {
                g=roiImage_.at<Vec3b>(i,j)[1];
                r=roiImage_.at<Vec3b>(i,j)[2];
                data[j]=maxVal(g,r);
            }
        }
}

//平滑滤波
void vetfarroadlightpreprocessor::getSmooth(Mat grayImage_,Mat& smoothImage_)
{
    //    blur(_grayImage,_grayImage,Size(3,3));//均值滤波
        GaussianBlur(grayImage_,smoothImage_,Size(3,3),3,3);//高斯滤波
    //   medianBlur(_grayImage,_grayImage,5);//中值滤波
}

//固定阈值二值化
void vetfarroadlightpreprocessor::getBinary(Mat smoothImage_,Mat& binaryImage_)
{
    binaryImage_.create(smoothImage_.size(),smoothImage_.type());
    threshold(smoothImage_,binaryImage_,VET_BinaryValue,255,0);
}

//RGB空间颜色方差消除自然光照的影响
void vetfarroadlightpreprocessor::eliminateWhiteLight(Mat roiImage_ ,Mat binaryImage_,Mat& pureImage_)
{
    for(int i=0;i<roiImage_.rows;i++)
    {
      uchar* data=binaryImage_.ptr<uchar>(i);
      for(int j=0;j<roiImage_.cols;j++)
      {
          int b=roiImage_.at<Vec3b>(i,j)[0];
          int g=roiImage_.at<Vec3b>(i,j)[1];
          int r=roiImage_.at<Vec3b>(i,j)[2];
          if(!(((abs(b-g)>70)||(abs(b-r)>70)||(abs(r-g)>70))))// is white color
          {
             data[j]=0;// put all black
            }
        }

    }
    pureImage_=binaryImage_.clone();
}

//形态学运算
void vetfarroadlightpreprocessor::getHomography(Mat pureImage_,Mat& homographyImage_)
{
    homographyImage_.create(pureImage_.size(),pureImage_.type());
    Mat element1 = getStructuringElement(MORPH_RECT, Size(3, 3));
    Mat element = getStructuringElement(MORPH_RECT, Size(1, 1));
 //   //开操作 (去除一些噪点)
    morphologyEx(pureImage_, pureImage_, MORPH_OPEN, element);
    dilate(pureImage_,homographyImage_,element1);//膨胀
}
