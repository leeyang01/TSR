#include"vetwarningsigndetection.h"
#include"vetwarningsignpreprocessor.h"
vetwarningsigndetector::vetwarningsigndetector()
{
    m_svm_warningClassifier.load("Warning1106.txt"); // 警告标志第一级分类器
    m_svm_shapeClassifier.load("warnSecond1108.txt"); // 警告标志第二级分类器
    m_fpsNum=0;
    m=0;
    m_slowdown=imread("slow.jpg");
    resize(m_slowdown,m_slowdown,Size(120,120));
    cvtColor(m_slowdown,m_slowdown_Mask,CV_RGB2GRAY);
    m_people=imread("people.jpg");
    resize(m_people,m_people,Size(120,120));
    cvtColor(m_people,m_people_Mask,CV_RGB2GRAY);
    m_child=imread("child.jpg");
    resize(m_child,m_child,Size(120,120));
    cvtColor(m_child,m_child_Mask,CV_RGB2GRAY);
}
vetwarningsigndetector::~vetwarningsigndetector()
{
    // 析构函数
}

// 警告标志检测接口
vector<warn> vetwarningsigndetector::warningdetector(Mat srcImage_)
{
    m_Warn_vector.clear();
    m_srcImage=srcImage_.clone();
    m_fpsNum++;
    vetwarningsignpreprocessor wp;
    wp.getROI(m_srcImage,m_roiImage);
    wp.warningProprecessor(m_roiImage,m_preprocessoredImage);
    findContour(m_preprocessoredImage,m_roiImage,m_contours);
    drawImage(m_roiImage,m_contours,m_Warn_vector,m_detectedImage);
    //addingROI(m_srcImage,m_detectedImage,m_dstImage);
    return m_Warn_vector;
}
// 警告标志轮廓查找，并进行轮廓分析
void vetwarningsigndetector::findContour(Mat preprocessorImage_,Mat roiImage_,vector<vector<Point> >& contours_)
{
    vector<Mat> frame;
    CvRect cvr;
    GaussianBlur(preprocessorImage_, preprocessorImage_, Size(5, 5), 0, 0);// 高斯滤波
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(preprocessorImage_, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));//找轮廓
    // 保留面积大于200并且小于3000的轮廓
    vector <vector<Point> >::iterator iter = contours.begin();
    for (; iter != contours.end();)
    {
        double g_dConArea = contourArea(*iter);

        // 轮廓面积筛选（200～2500）
        if (g_dConArea >=200&&g_dConArea<=2500)
        {
            ++iter;
        }
        else
        {
            iter = contours.erase(iter);
        }
    }
   // 最小外界矩形宽高比筛选
   vector <vector<Point> >::iterator iter_radio = contours.begin();
   for (; iter_radio != contours.end();)
   {
       RotatedRect rect = minAreaRect(*iter_radio);
       Point2f P[4];
       rect.points(P);
       float d1 = sqrt((P[1].y - P[0].y)*(P[1].y - P[0].y) + (P[1].x - P[0].x)*(P[1].x - P[0].x));
       float d2 = sqrt((P[2].y - P[1].y)*(P[2].y - P[1].y) + (P[2].x - P[1].x)*(P[2].x - P[1].x));
       if (d1 / d2 >= 0.8&&d1 / d2 <= 1.2)
       {
           ++iter_radio;
       }
       else
       {
           iter_radio = contours.erase(iter_radio);
       }
   }
   // 边界越界筛选
   vector <vector<Point> >::iterator iter_boundary = contours.begin();
   for (; iter_boundary != contours.end();)
   {
       Point2f center; float radius;
       minEnclosingCircle(*iter_boundary, center, radius);
       cvr.x = center.x-radius-1;
       cvr.y =center.y-radius-1;
       cvr.height = 2 * radius + 2;
       cvr.width = 2 * radius + 2;
       if ((cvr.x>0&&cvr.y>0&&(cvr.x+cvr.width)<preprocessorImage_.cols&&(cvr.y+cvr.height)<preprocessorImage_.rows))
       {
           ++iter_boundary;
       }
       else
       {
           iter_boundary = contours.erase(iter_boundary);
       }
   }

   // 第一级分类器筛选
   vector <vector<Point> >::iterator iter_SVM = contours.begin();
   for (; iter_SVM != contours.end();)
   {
       Point2f center; float radius;
       minEnclosingCircle(*iter_SVM, center, radius);
       cvr.x = center.x-radius;
       cvr.y =center.y-radius;
       cvr.height = 2 * radius ;
       cvr.width = 2 * radius;
       Mat outImage=roiImage_(cvr);//警告标志，截取原图
       resize(outImage,outImage,Size(72,72));//归一到72*72
       int label=warningSVM(outImage);//第一级分类器，识别是否为交通信号灯
       if (label==1)
       {
           ++iter_SVM;
           frame.push_back(outImage);
       }
       else
       {
           iter_SVM = contours.erase(iter_SVM);
       }
   }
   contours_=contours;
//           for(int i=0;i<frame.size();i++){
//               char name[200];
//               sprintf(name, "%s%.6d%s", "/media/lee/Lee-Disk/TSR文件夹/警告标志检测与识别/Data/警告标志训练集/第一级分类器（识别是否为警告标志）/warn frame/", ++m, ".jpg");//保存的图片名
//               imwrite(name,frame[i]);
//           }
}

//画出检测到的警告标志
void vetwarningsigndetector::drawImage(Mat roiImage_, vector<vector<Point> > contours_,vector<warn>& Warn_vector_,Mat& detectedImage_)
{
    detectedImage_=roiImage_.clone();
    CvRect cvr,cvr1;
    for (int i = 0; i < (int)contours_.size(); i++)
    {
        Point2f center; float radius;
        minEnclosingCircle(contours_[i], center, radius);
        cvr.x = center.x-radius-1;
        cvr.y =center.y-radius-1;
        cvr.height = 2 * radius+2 ;
        cvr.width = 2 * radius+2;
        cvr1.x=cvr.x+480;
        cvr1.y=cvr.y+250;
        cvr1.height=cvr.height;
        cvr1.width=cvr.width;
        Mat meaning_img=detectedImage_(cvr);
        resize(meaning_img,meaning_img,Size(72,72));
        int meaning=warningShape(meaning_img);
        switch (meaning)
        {
        case 1:
            m_Warn.m_warnFPS=m_fpsNum;
            m_Warn.m_warnPoint=center;
            m_Warn.m_warnRect=cvr1;
            m_Warn.m_label=1;
            Warn_vector_.push_back(m_Warn);
            break;
        case 2:
            m_Warn.m_warnPoint=center;
            m_Warn.m_warnRect=cvr1;
            m_Warn.m_label=2;
            Warn_vector_.push_back(m_Warn);
            break;
        case 3:
            m_Warn.m_warnFPS=m_fpsNum;
            m_Warn.m_warnPoint=center;
            m_Warn.m_warnRect=cvr1;
            m_Warn.m_label=3;
            Warn_vector_.push_back(m_Warn);
            break;
        case 4:
            m_Warn.m_warnFPS=m_fpsNum;
            m_Warn.m_warnPoint=center;
            m_Warn.m_warnRect=cvr1;
            m_Warn.m_label=4;
            Warn_vector_.push_back(m_Warn);
            break;
        default:
            break;
        }
        rectangle(detectedImage_, cvr, Scalar(0, 255, 255),2);//识别信号灯为红色就用红色框显示出来
    }
}

//HOG+SVM判断是否为WarningSign,1 is true,-1 is false
int vetwarningsigndetector::warningSVM(Mat candidate_area_)
{
    HOGDescriptor *hog=new HOGDescriptor(cvSize(72,72),cvSize(12,12),cvSize(6,6),cvSize(6,6),9);
    vector<float>descriptors;//结果数组
    hog->compute(candidate_area_, descriptors,Size(6,6), Size(0,0));
    Mat SVMtrainMat = Mat::zeros(1,descriptors.size(),CV_32FC1);
    int n=0;

    for(vector<float>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)
    {
            SVMtrainMat.at<float>(0,n) = *iter;
            n++;
    }
    int ret = m_svm_warningClassifier.predict(SVMtrainMat);//检测结果
    return ret;
}

//HOG+SVM判断是否为Warningmeaning,1 is slowdown,2 is people,3 is student
int vetwarningsigndetector::warningShape(Mat candidate_area_)
{
    resize(candidate_area_,candidate_area_,Size(72,72));
    HOGDescriptor *hog=new HOGDescriptor(cvSize(72,72),cvSize(12,12),cvSize(6,6),cvSize(6,6),9);
    vector<float>descriptors;//结果数组
    hog->compute(candidate_area_, descriptors,Size(2,2), Size(0,0));
    Mat SVMtrainMat = Mat::zeros(1,descriptors.size(),CV_32FC1);
    int n=0;

    for(vector<float>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)
    {
            SVMtrainMat.at<float>(0,n) = *iter;
            n++;
    }
    int ret = m_svm_shapeClassifier.predict(SVMtrainMat);//检测结果
    return ret;
}

////加载ROI到原图
//void vetwarningsigndetector::addingROI(Mat srcImage_, Mat detectedImage_,Mat& dstImage_)
//{
//    dstImage_=srcImage_.clone();
//    Rect rect=Rect(VET_WARN_X,VET_WARN_Y,VET_WARN_W,VET_WARN_H);
//    Mat srcROI=dstImage_(rect);
//    Mat mask;
//    cvtColor(detectedImage_,mask,CV_RGB2GRAY);
//    detectedImage_.copyTo(srcROI,mask);
//    Mat imageROI=dstImage_(Rect(430,600,120,120));
//    switch(m_Warn_vector.size()){
//    case 1:
//        switch (m_Warn_vector[0].m_warnMeaning)
//        {
//        case 1:
//            m_slowdown.copyTo(imageROI,m_slowdown_Mask);
//            break;
//        case 2:
//            m_people.copyTo(imageROI,m_people_Mask);
//            break;
//        case 3:
//            m_child.copyTo(imageROI,m_child_Mask);
//            break;
//        default:
//            break;
//        }
//        break;
//    case 2:
//        break;
//    default:
//        break;
//    }
//    //putText(_srcImage,"Warning",Point(X1+400,Y1-2),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);
//    //rectangle(dstImage_,Rect(430,600,120,120),Scalar(255,255,255));
//    m_Warn_vector.clear();
//}
