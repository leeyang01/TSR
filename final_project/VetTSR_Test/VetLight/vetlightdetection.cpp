#include"vetlightdetection.h"
#include"vetlightpreprocessor.h"
vetlightdetection::vetlightdetection()
{// 构造函数
    m_svm_lightClassifier.load("1016Light_SVM.txt");// 信号灯第一级分类器
    m_svm_shapeClassifier.load("shape0909.txt");// 信号灯第二级分类器
    m_svm_blackboxClassifier.load("BlockBox1014addneg.txt");// 形状分类器
    m_file.open("/home/lee/Desktop/Light/file/HPIM0092/result.csv", ofstream::out);// 信号灯输出文件
    m_file<<"fps,point,Area,Color\n";//文件头
    m_upNum=0;
    m_leftNum=0;
    m_rightNum=0;
    m_fpsNum=0;
    m_count=0;
    m_showCount=0;
    m_lightNum=0;
}
vetlightdetection::~vetlightdetection()
{
	//析构函数
}

// 信号灯检测与识别接口
vector<trafficlight> vetlightdetection::lightDetecte(Mat srcImage_)
{
    m_Light_vector.clear();
    m_srcImage=srcImage_.clone();
    m_fpsNum++;
    vetfarroadlightpreprocessor vp;
    vp.getROIImage(m_srcImage,m_roiImage);
    vp.lightPreprocessor(m_roiImage,m_preprocessedImage);
    findContour(m_preprocessedImage,m_roiImage,m_contours,m_Light_vector);
    drawImage(m_roiImage,m_contours,m_detectedImage);
    //addingROI(m_srcImage,m_detectedImage,m_dstImage);
    srcImage_=m_dstImage.clone();
    return m_Light_vector;
}

inline int vetlightdetection::maximum(int a,int b,int c)// 内联函数，取三个参数的最大值
{
    if(a>=b){
        if(a>=c)
        {
            return 1;
        }else
        {
            return 3;
        }

    }else{
        if(b>=c)
        {
            return 2;
        }
        else{
            return 3;
        }
    }
}
// 信号灯轮廓查找并分析（面积，半径以及宽高比）
void vetlightdetection::findContour(Mat homographyImage_,Mat roiImage_,vector<vector<Point> >& contours_,vector<trafficlight> & light_vector_)
{
    GaussianBlur(homographyImage_, homographyImage_, Size(5, 5), 0, 0);// 高斯滤波
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(homographyImage_, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));//找轮廓
    // 绘制轮廓
    for (int i = 0; i < (int)contours.size(); i++)
    {
        drawContours(homographyImage_, contours, i, Scalar(255), 1, 8);
    }
    // 保留面积大于30并且小于250的轮廓
    vector <vector<Point> >::iterator iter = contours.begin();
    for (; iter != contours.end();)
    {
        double g_dConArea = contourArea(*iter);
        // 轮廓面积筛选（30～250）
        if (g_dConArea >=30&&g_dConArea<=250)
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

    // 最小外接圆半径筛选
    vector <vector<Point> >::iterator iter_radius = contours.begin();
    for (; iter_radius != contours.end();)
    {
        Point2f center; float radius;
        minEnclosingCircle(*iter_radius, center, radius);
        if (radius >=3&&radius<=30)
        {
            ++iter_radius;
        }
        else
        {
            iter_radius = contours.erase(iter_radius);
        }
    }

    // 边界越界筛选
    vector <vector<Point> >::iterator iter_boundary = contours.begin();
    for (; iter_boundary != contours.end();)
    {
        CvRect _cvr;
        Point2f center; float radius;
        minEnclosingCircle(*iter_boundary, center, radius);
        _cvr.x = center.x-radius-2;
        _cvr.y =center.y-radius-2;
        _cvr.height = 2 * radius + 6;
        _cvr.width = 2 * radius + 6;
        if ((_cvr.x>0&&_cvr.y>0&&(_cvr.x+_cvr.width)<homographyImage_.cols&&(_cvr.y+_cvr.height)<homographyImage_.rows))
        {
            ++iter_boundary;
        }
        else
        {
            iter_boundary = contours.erase(iter_boundary);
        }
    }
    // 第一级分类器筛选
    vector <vector<Point> >::iterator iter_HOG_Light = contours.begin();
    for (; iter_HOG_Light != contours.end();)
    {
        CvRect _cvr;
        Point2f center; float radius;
        minEnclosingCircle(*iter_HOG_Light, center, radius);
        _cvr.x = center.x-radius-1;
        _cvr.y =center.y-radius-1;
        _cvr.height = 2 * radius + 4;
        _cvr.width = 2 * radius + 4;
        Mat outImage=roiImage_(_cvr);// 信号灯外框显示，截取原图
        resize(outImage,outImage,Size(48,48));// 归一到48*48
        int label=lightSVM(outImage);// 第一级分类器，识别是否为交通信号灯
        if (label==1)
        {
            ++iter_HOG_Light;
        }
        else
        {
            iter_HOG_Light = contours.erase(iter_HOG_Light);
        }
    }
    // BlackBox筛选
    vector <vector<Point> >::iterator iter_BlackBox = contours.begin();
    for (; iter_BlackBox != contours.end();)
    {
        CvRect cvr_Rect,cvr_S,cvr_H;
        Point2f center; float radius;
        minEnclosingCircle(*iter_BlackBox, center, radius);
        cvr_Rect.x = center.x-radius-1;
        cvr_Rect.y =center.y-radius-1;
        cvr_Rect.height = 2 * radius + 2;
        cvr_Rect.width = 2 * radius + 2;
        Mat out1=roiImage_(cvr_Rect);
        Mat out_S,out_H;
        double Str=0,Hor=0;
        int color=colorRecognise(out1);
        switch (color)
        {
        case 1://Red
            m_Light.m_color=Scalar(0,0,255);
            m_Light.m_shape="Stop";
            cvr_S.x=center.x-1.2*radius;
            cvr_S.y=center.y-1.2*radius;
            cvr_S.height=5.4 * radius;
            cvr_S.width=2.7* radius;
            cvr_H.x=center.x-radius;
            cvr_H.y=center.y-radius;
            cvr_H.height=2.5 * radius;
            cvr_H.width=5.5 * radius;
            if(cvr_S.x<=0)
            {
                cvr_S.x=0;
            }
            if(cvr_S.y<=0)
            {
                cvr_S.y=0;
            }
            if(cvr_S.y+cvr_S.height>=roiImage_.rows)
            {
                cvr_S.height=roiImage_.rows-cvr_S.y;
            }
            if(cvr_S.x+cvr_S.width>=roiImage_.cols)
            {
                cvr_S.width=roiImage_.cols-cvr_S.x;
            }
            out_S=roiImage_(cvr_S);
            if(cvr_H.x<=0){
                cvr_H.x=0;
            }
            if(cvr_H.y<=0){
                cvr_H.y=0;
            }
            if(cvr_H.y+cvr_H.height>=roiImage_.rows)
            {
                cvr_H.height=roiImage_.rows-cvr_H.y;
            }
            if(cvr_H.x+cvr_H.width>=roiImage_.cols)
            {
                cvr_H.width=roiImage_.cols-cvr_H.x;
            }
            out_H=roiImage_(cvr_H);
            break;
        case 2://Green
            m_Light.m_color=Scalar(0,255,0);
            m_Light.m_shape="Go";
            cvr_S.x=center.x-1.2*radius;
            cvr_S.y=center.y-4.1*radius;
            cvr_S.height=5.5 * radius;
            cvr_S.width=2.8 * radius;
            cvr_H.x=center.x-4*radius;
            cvr_H.y=center.y-radius;
            cvr_H.height=2.5* radius;
            cvr_H.width=5.5 * radius;
            if(cvr_S.x<=0)
            {
                cvr_S.x=0;
            }
            if(cvr_S.y<=0)
            {
                cvr_S.y=0;
            }
            if(cvr_S.y+cvr_S.height>=roiImage_.rows)
            {
                cvr_S.height=roiImage_.rows-cvr_S.y;
            }
            if(cvr_S.x+cvr_S.width>=roiImage_.cols)
            {
                cvr_S.width=roiImage_.cols-cvr_S.x;
            }
            out_S=roiImage_(cvr_S);
            if(cvr_H.x<=0)
            {
                cvr_H.x=0;
            }
            if(cvr_H.y<=0)
            {
                cvr_H.y=0;
            }
            if(cvr_H.y+cvr_H.height>=roiImage_.rows)
            {
                cvr_H.height=roiImage_.rows-cvr_H.y;
            }
            if(cvr_H.x+cvr_H.width>=roiImage_.cols)
            {
                cvr_H.width=roiImage_.cols-cvr_H.x;
            }
            out_H=roiImage_(cvr_H);
            break;
        case 3://Yellow
            m_Light.m_color=Scalar(0,255,255);
            m_Light.m_shape="Warn";
            cvr_S.x=center.x-radius;
            cvr_S.y=center.y-3*radius;
            cvr_S.height=5.5 * radius;
            cvr_S.width=2.5* radius ;
            cvr_H.x=center.x-3*radius;
            cvr_H.y=center.y-radius;
            cvr_H.height=2.5* radius ;
            cvr_H.width=5.5 * radius;
            if(cvr_S.x<=0)
            {
                cvr_S.x=0;
            }
            if(cvr_S.y<=0)
            {
                cvr_S.y=0;
            }
            if(cvr_S.y+cvr_S.height>=roiImage_.rows)
            {
                cvr_S.height=roiImage_.rows-cvr_S.y;
            }
            if(cvr_S.x+cvr_S.width>=roiImage_.cols)
            {
                cvr_S.width=roiImage_.cols-cvr_S.x;
            }
            out_S=roiImage_(cvr_S);
            if(cvr_H.x<=0)
            {
                cvr_H.x=0;
            }
            if(cvr_H.y<=0)
            {
                cvr_H.y=0;
            }
            if(cvr_H.y+cvr_H.height>=roiImage_.rows)
            {
                cvr_H.height=roiImage_.rows-cvr_H.y;
            }
            if(cvr_H.x+cvr_H.width>=roiImage_.cols)
            {
                cvr_H.width=roiImage_.cols-cvr_H.x;
            }
            out_H=roiImage_(cvr_H);
            break;
        default:
            break;
        }
        Str=BlackBox(out_S);
        Hor=BlackBox(out_H);
        resize(out_S,out_S,Size(24,72));
        int lable_blackbox=blackboxSVM(out_S);
        if(Str>Hor&&Str>0&&lable_blackbox==1)
        {
            ++iter_BlackBox;
            m_Light.m_fps=m_fpsNum;
            m_Light.m_point=center;
            cvr_S.x=cvr_S.x+380;
            cvr_S.y=cvr_S.y+230;
            m_Light.m_Rect=cvr_S;
            m_Light.m_direction="Straight";
            m_Light_vector.push_back(m_Light);
            //cout<<"fps: "<<Light._fps<<" x: "<<Light._point.x<<" y: "<<Light._point.y<<" color:"<<Light._color<<" Dir: "<<Light._direction<<endl;
        }
//         if(Hor>Str&&Hor>0){
//            ++iter_BlackBox;
//            Light._fps=fpsNum;
//            Light._point=center;
//            Light._Rect=cvr_H;
//            Light._direction="Horizon";
//            cout<<"fps: "<<Light._fps<<" x: "<<Light._point.x<<" y: "<<Light._point.y<<" color:"<<Light._color<<" Dir: "<<Light._direction<<endl;
//        }
        else
        {
             iter_BlackBox = contours.erase(iter_BlackBox);
        }

    }
//    for(int i=0;i<_Light.size();i++){
//        cout<<_Light[i]._point.x<<" ";
//    }
//    cout<<endl;
    //cout<<"BlackBox筛选: "<<contours.size()<<endl;
    light_vector_=m_Light_vector;
    contours_=contours;
}

// HOG+SVM判断是否为交通灯，是-1，不是1
int vetlightdetection::lightSVM(Mat candidate_area_)
{
    HOGDescriptor *hog=new HOGDescriptor(cvSize(48,48),cvSize(8,8),cvSize(4,4),cvSize(4,4),9);
    vector<float>descriptors;// 结果数组
    hog->compute(candidate_area_, descriptors,Size(2,2), Size(0,0));
    Mat SVMtrainMat = Mat::zeros(1,descriptors.size(),CV_32FC1);
    int n=0;

    for(vector<float>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)
    {
        SVMtrainMat.at<float>(0,n) = *iter;
        n++;
    }
    int ret = m_svm_lightClassifier.predict(SVMtrainMat); //检测结果
    return ret;
}

// HOG+SVM 判断交通信号灯形状，up，left，right，circle
int vetlightdetection::shapeSVM(Mat candidate_area_)
{
    HOGDescriptor *hog=new HOGDescriptor(cvSize(48,48),cvSize(8,8),cvSize(4,4),cvSize(4,4),9);
    vector<float>descriptors;// 结果数组
    hog->compute(candidate_area_, descriptors,Size(2,2), Size(0,0));
    Mat SVMtrainMat = Mat::zeros(1,descriptors.size(),CV_32FC1);
    int n=0;

    for(vector<float>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)
    {
        SVMtrainMat.at<float>(0,n) = *iter;
        n++;
    }
    int ret = m_svm_shapeClassifier.predict(SVMtrainMat);// 检测结果
    return ret;
}

// HOG+SVM判断BlackBox是否为交通灯，是-1，不是1
int vetlightdetection::blackboxSVM(Mat candidate_area_)
{
    resize(candidate_area_,candidate_area_,Size(24,72));
    HOGDescriptor *hog=new HOGDescriptor(cvSize(24,72),cvSize(4,12),cvSize(2,6),cvSize(2,6),9);
    vector<float>descriptors;// 结果数组
    hog->compute(candidate_area_, descriptors,Size(1,3), Size(0,0));
    Mat SVMtrainMat = Mat::zeros(1,descriptors.size(),CV_32FC1);
    int n=0;

    for(vector<float>::iterator iter=descriptors.begin();iter!=descriptors.end();iter++)
    {
        SVMtrainMat.at<float>(0,n) = *iter;
        n++;
    }
    int ret = m_svm_blackboxClassifier.predict(SVMtrainMat);// 检测结果
    return ret;
}

// 基于RGB空间像素统计的颜色识别
int vetlightdetection::colorRecognise(Mat candidate_area_)
{
    int blue=0, green=0, red=0;
    int y=0,g=0,r=0;
    float R, G, B;
    for (int i = 0; i < candidate_area_.rows; i++)
    {
        for (int j = 0; j < candidate_area_.cols; j++)
        {
            blue = candidate_area_.at<Vec3b>(i, j)[0];
            green = candidate_area_.at<Vec3b>(i, j)[1];
            red = candidate_area_.at<Vec3b>(i, j)[2];
            R = (float)(red) / (float)(blue + green + red);
            G = (float)(green) / (float)(blue + green + red);
            B = (float)(blue) / (float)(blue + green + red);
            if ((R-G)>0.08 && (R-B)>0.08 && (G - B)<0.08&&(R / G>1.4))
            {// 红色
                r++;

            }
            if((R-B)>0.12&&(G-B)>0.12)
            {// 黄色
                y++;
            }
            if((G-R)>0.12&&(G-B)>0.02&&(B-R)>0.08)
            {// 绿色
                g++;
            }

        }
    }

    int c=maximum(r,g,y);
    return c;
}
// 统计信号灯背板框黑色像素
double vetlightdetection::BlackBox(Mat image)
{
    int blue=0, green=0, red=0;
    double black=0,p=0;
    //float R, G, B;
    for (int i = 0; i < image.rows; i++)
    {
        for (int j = 0; j < image.cols; j++)
    {
            blue = image.at<Vec3b>(i, j)[0];
            green = image.at<Vec3b>(i, j)[1];
            red = image.at<Vec3b>(i, j)[2];
//            R = (float)(red) / (float)(blue + green + red);
//            G = (float)(green) / (float)(blue + green + red);
//            B = (float)(blue) / (float)(blue + green + red);
            //s=(R-1/3)*(R-1/3)+(G-1/3)*(G-1/3)+(B-1/3)*(B-1/3);
            if(red<60&&green<60&&blue<60)
    {
                black++;
            }
       }
   }
    p=black/(image.rows*image.cols);
    return p;
}
// 画检测到的信号灯
void vetlightdetection::drawImage(Mat roiImage_,vector<vector<Point> > contours_,Mat& detectedImage_){
    detectedImage_=roiImage_.clone();
    for(int i = 0; i < (int)contours_.size(); i++ )
    {
        Mat shapeImage,colorImage,lightImage;
        CvRect _cvr,_cvr1,cvr_S;
        Point2f center; float radius;
        minEnclosingCircle(contours_[i], center, radius);
        _cvr.x = center.x-radius-2;
        _cvr.y =center.y-radius-2;
        _cvr.height = 2 * radius + 6;
        _cvr.width = 2 * radius + 6;
        _cvr1.x = center.x-radius-2;
        _cvr1.y =center.y-radius-2;
        _cvr1.height = 2 * radius + 6;
        _cvr1.width = 2 * radius + 6;
        lightImage=detectedImage_(_cvr1);
        colorImage=detectedImage_(_cvr);
        resize(lightImage,lightImage,Size(48,48));
        resize(colorImage,colorImage,Size(48,48));
        shapeImage.create(colorImage.size(),colorImage.type());
        vetfarroadlightpreprocessor v;
        v.getMaxGray(colorImage,shapeImage);// 信号灯形状分类小框灰度化
        double Area = contourArea(contours_[i]);// 计算轮廓的面积
        GaussianBlur(shapeImage,shapeImage,Size(3,3),3,3);// 高斯滤波
        threshold(shapeImage,shapeImage,VET_BinaryValue,255,0);// 固定阈值二值化
        //_labelNum=lightSVM(lightImage);// 第一级分类器，识别是否为交通信号灯
        m_shapeNum=shapeSVM(shapeImage);// 第二级分类器，识别交通信号灯形状
        m_colorNum=colorRecognise(colorImage);//第三识别交通信号灯颜色
        m_isShow=false;
//        if (_labelNum==1)//判别是信号灯
        {
//                char name[100];
//                sprintf(name, "%s%.6d%s", "/home/lee/Desktop/light_out/image", ++m, ".jpg");//保存的图片名
//                imwrite(name,lightImage);

            //if(_lightNum>=2)
            {
//            Light._fps=fpsNum;
//            Light._point.x=center.x;
//            Light._point.y=center.y;
//            Light._area=Area;
//            file<<Light._fps<<","<<Light._point.x<<";"<<Light._point.y<< ","<<Light._area<<",";
            switch(m_colorNum)//判断信号灯颜色
            {
            case 1://Red
                cvr_S.x=center.x-radius;
                cvr_S.y=center.y-radius;
                cvr_S.height=5.5 * radius;
                cvr_S.width=2.5* radius;
                //Light._color="Red";
                //file<<Light._color<<endl;
                switch(m_shapeNum)//判别信号灯形状
                {
                case 1://up
                    m_upNum++;//统计检测帧数
                    if(m_upNum>=3&&Area>=80)//连续检测到三帧图像并且轮廓面积大于80，才显示识别到的形状
                    {
                        putText(detectedImage_,"Up",Point(cvr_S.x-2,cvr_S.y-5),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);
                        m_upNum=0;//显示完统计值清零
                        m_isShow=true;
                    }
                    break;
                case 2://left
                    m_leftNum++;//统计检测帧数
                    if(m_leftNum>=3&&Area>=80)//连续检测到三帧图像并且轮廓面积大于80，才显示识别到的形状
                    {
                        putText(detectedImage_,"left",Point(cvr_S.x-2,cvr_S.y-5),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);
                        m_leftNum=0;//显示完统计值清零
                        m_isShow=true;
                    }
                    break;
                case 3://right
                    m_rightNum++;//统计检测帧数
                    if(m_rightNum>=3&&Area>=80)//连续检测到三帧图像并且轮廓面积大于80，才显示识别到的形状
                    {
                        putText(detectedImage_,"right",Point(cvr_S.x-2,cvr_S.y-5),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);
                        m_rightNum=0;//显示完统计值清零
                        m_isShow=true;
                    }
                    break;
                case 4://circle
                    //putText(src,"Stop!!!",Point(cvr.x-2,cvr.y-5),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);
                    break;
                default:
                    break;
                }
                if(m_isShow==false){
                    putText(detectedImage_,"Stop",Point(cvr_S.x-2,cvr_S.y-5),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 0, 255), 2);
                }
                rectangle(detectedImage_, cvr_S, Scalar(0, 0, 255),2);//识别信号灯为红色就用红色框显示出来
                break;

            case 2://Green
                cvr_S.x=center.x-radius;
                cvr_S.y=center.y-4*radius;
                cvr_S.height=5.5 * radius;
                cvr_S.width=2.5 * radius;
//                Light._color="Green";
//                file<<Light._color<<endl;
                switch(m_shapeNum)//判别信号灯形状
                {
                case 1://up
                    m_upNum++;//统计检测帧数
                    if(m_upNum>=3&&Area>=80)//连续检测到三帧图像并且轮廓面积大于80，才显示识别到的形状
                    {
                        putText(detectedImage_,"Up",Point(cvr_S.x-2,cvr_S.y-5),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 0), 2);
                        m_upNum=0;//显示完统计值清零
                        m_isShow=true;
                    }
                    break;
                case 2://left
                    m_leftNum++;//统计检测帧数
                    if(m_leftNum>=3&&Area>=80)//连续检测到三帧图像并且轮廓面积大于80，才显示识别到的形状
                    {
                        putText(detectedImage_,"left",Point(cvr_S.x-2,cvr_S.y-5),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 0), 2);
                        m_leftNum=0;//显示完统计值清零
                        m_isShow=true;
                    }
                    break;
                case 3://right
                    m_rightNum++;//统计检测帧数
                    if(m_rightNum>=3&&Area>=80)//连续检测到三帧图像并且轮廓面积大于80，才显示识别到的形状
                    {
                        putText(detectedImage_,"right",Point(cvr_S.x-2,cvr_S.y-5),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 0), 2);
                        m_rightNum=0;//显示完统计值清零
                        m_isShow=true;
                    }
                    break;
                case 4://circle
                    //putText(src,"Go!!!",Point(cvr.x-2,cvr.y-5),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 0), 2);
                    break;
                default:
                    break;
                }
                if(m_isShow==false)
                {
                    putText(detectedImage_,"Go",Point(cvr_S.x-2,cvr_S.y-5),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 0), 2);
                }
                rectangle(detectedImage_, cvr_S, Scalar(0, 255, 0),2);//识别信号灯为绿色就用绿色框显示出来
                break;
            case 3://Yellow
                cvr_S.x=center.x-radius;
                cvr_S.y=center.y-3*radius;
                cvr_S.height=5.5 * radius;
                cvr_S.width=2.5* radius ;
//                Light._color="Yellow";
//                file<<Light._color<<endl;
                switch(m_shapeNum)//判别信号灯形状
                {
                case 1://up
                    m_upNum++;//统计检测帧数
                    if(m_upNum>=3&&Area>=80)//连续检测到三帧图像并且轮廓面积大于80，才显示识别到的形状
                    {
                        putText(detectedImage_,"Up",Point(cvr_S.x-2,cvr_S.y-5),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 255), 2);
                        m_upNum=0;//显示完统计值清零
                        m_isShow=true;
                    }
                    break;
                case 2://left
                    m_leftNum++;//统计检测帧数
                    if(m_leftNum>=3&&Area>=80)//连续检测到三帧图像并且轮廓面积大于80，才显示识别到的形状
                    {
                        putText(detectedImage_,"left",Point(cvr_S.x-2,cvr_S.y-5),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 255),2);
                        m_leftNum=0;//显示完统计值清零
                        m_isShow=true;
                    }
                    break;
                case 3://right
                    m_rightNum++;//统计检测帧数
                    if(m_rightNum>=3&&Area>=80)//连续检测到三帧图像并且轮廓面积大于80，才显示识别到的形状
                    {
                        putText(detectedImage_,"right",Point(cvr_S.x-2,cvr_S.y-5),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 255), 2);
                        m_rightNum=0;//显示完统计值清零
                        m_isShow=true;
                    }
                    break;
                case 4://circle
                    //putText(src,"Slow!!!",Point(cvr.x-2,cvr.y-2),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 255), 2);
                    break;
                default:
                    break;
                }
                if(m_isShow==false)
                {
                    putText(detectedImage_,"Warn",Point(cvr_S.x-2,cvr_S.y-2),FONT_HERSHEY_SIMPLEX, 0.6, Scalar(0, 255, 255), 2);
                }
                rectangle(detectedImage_, cvr_S, Scalar(0, 255, 255),2);//识别信号灯为黄色就用黄色框显示出来
                break;
            default:
                break;
            }

          }

        }
        //file<<Light._fps<<","<<Light._point.x<<";"<<Light._point.y<< ","<<Light._area<<","<<Light._color<<"\n";

        //file<<Light._fps<<","<<Light._point.x<<";"<<Light._point.y<< ","<<Light._area<<","<<"Null"<<"\n";
        //cout<<Light._fps<<endl;

    }

}

//画整张图像的效果
void vetlightdetection::addingROI(Mat srcImage_,Mat detectedImage_,Mat& dstImage_)
{
    dstImage_=srcImage_.clone();
    Rect rect=Rect(VET_LIGHT_X,VET_LIGHT_Y,VET_LIGHT_W,VET_LIGHT_H);
    Mat srcROI=dstImage_(rect);
    Mat mask;
    cvtColor(detectedImage_,mask,CV_RGB2GRAY);
    detectedImage_.copyTo(srcROI,mask);
    rectangle(dstImage_,Rect(550,660,180,60),Scalar(255,255,255));
    switch (m_Light_vector.size())
    {
    case 1:
        circle(dstImage_,Point(640,690),20,m_Light_vector[0].m_color,-1,8);
        circle(dstImage_,Point(580,690),20,Scalar(0,0,0),-1,8);
        circle(dstImage_,Point(700,690),20,Scalar(0,0,0),-1,8);
        m_Scalar.push_back(m_Light_vector[0].m_color);
        m_lightNum=1;
        m_showCount=5;
        break;
    case 2:       
        circle(dstImage_,Point(580,690),20,m_Light_vector[1].m_color,-1,8);
        circle(dstImage_,Point(640,690),20,m_Light_vector[0].m_color,-1,8);
        circle(dstImage_,Point(700,690),20,Scalar(0,0,0),-1,8);
        m_lightNum=2;
        m_showCount=5;
        break;
    case 3:
        circle(dstImage_,Point(580,690),20,m_Light_vector[2].m_color,-1,8);
        circle(dstImage_,Point(640,690),20,m_Light_vector[1].m_color,-1,8);
        circle(dstImage_,Point(700,690),20,m_Light_vector[0].m_color,-1,8);
        m_lightNum=3;
        m_showCount=5;
        break;
    default:
        circle(dstImage_,Point(580,690),20,Scalar(0,0,0),-1,8);
        circle(dstImage_,Point(640,690),20,Scalar(0,0,0),-1,8);
        circle(dstImage_,Point(700,690),20,Scalar(0,0,0),-1,8);
        break;
    }
    m_Light_vector.clear();
}

