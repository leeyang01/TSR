#include"vetwarningsignpreprocessor.h"
vetwarningsignpreprocessor::vetwarningsignpreprocessor(){
    // 默认构造函数
}
vetwarningsignpreprocessor::~vetwarningsignpreprocessor(){
    // 默认析构函数
}

// 警告标志预处理接口
void vetwarningsignpreprocessor::warningProprecessor(Mat roiImage_,Mat& preprocessoredImage_)
{
    m_roiImage=roiImage_.clone();
    getEqualizeCorrect(m_roiImage,m_equalizeImage);
    splitYellow(m_equalizeImage,m_preprocessoredImage);
    preprocessoredImage_=m_preprocessoredImage.clone();
}
// 截取ROI区域
void vetwarningsignpreprocessor::getROI(Mat srcImage_,Mat& roiImage_)
{
    roiImage_=srcImage_(Rect(VET_WARN_X,VET_WARN_Y,VET_WARN_W,VET_WARN_H));
}
// RGB三通道分别均衡化
void vetwarningsignpreprocessor::getEqualizeCorrect(Mat roiImage_,Mat &equalizeImage_)
{
    Mat imageRGB[3];
    split(roiImage_, imageRGB);
    for (int i = 0; i < 3; i++)
      {
         equalizeHist(imageRGB[i], imageRGB[i]);
      }
    merge(imageRGB, 3, equalizeImage_);
}

// 利用RGB空间颜色差分割黄色
void vetwarningsignpreprocessor::splitYellow(Mat equalizeImage_,Mat& preprocessoredImage_)
{
    preprocessoredImage_=Mat::zeros(equalizeImage_.size(),CV_8UC1);
    int blue=0, green=0, red=0;
    float R, G, B;
    for (int i = 0; i < equalizeImage_.rows; i++)
    {
        uchar* data = preprocessoredImage_.ptr<uchar>(i);
        for (int j = 0; j < equalizeImage_.cols; j++)
        {
            blue = equalizeImage_.at<Vec3b>(i, j)[0];
            green = equalizeImage_.at<Vec3b>(i, j)[1];
            red = equalizeImage_.at<Vec3b>(i, j)[2];
            R = (float)(red) / (float)(blue + green + red);
            G = (float)(green) / (float)(blue + green + red);
            B = (float)(blue) / (float)(blue + green + red);
            if((R-B)>0.12&&(G-B)>0.12)
            { // RGB空间判断黄色的条件
              data[j] =255; // 像素值满足条件，创建一个掩图，置为白
            }
            else
            {
              data[j]=0; // 像素值不满足条件，置为黑
            }
       }
   }
}
