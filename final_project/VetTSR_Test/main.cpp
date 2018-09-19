#include"vetlightpreprocessor.h"
#include"vetlightdetection.h"
#include"vetwarningsigndetection.h"
#include"vetwarningsignpreprocessor.h"
#include"vetstatemachine.h"
#include"vetsigndetector.h"
#include"vettrackermodel.h"
#include"struct.h"
#include<pthread.h>
using namespace std;
ofstream file;
double fps;
double t;
bool stop;
int m;
int rate ;
int rate_base;
bool pause;
Mat srcImage;
int number;
pthread_mutex_t image_mutex;
Mat dstImge;
VetFarRoadSignPreprocessor signpreprocessor;
VetSignDetector sd;
Mat signprepframe;
Mat signImage;
VettrackerModel vt;
vetlightdetection vl;
vetwarningsigndetector vw;
vector<trafficlight> Light_vector;
vector<warn> Warn_vector;
vector<RectRegion> sign_rect_region;
void *function_light(void*args){//信号灯线程
    Light_vector.clear();
    vt.TrafficLight(srcImage,Light_vector);
    pthread_exit(0);
}
void *warning(void*args){// 警告标志线程
    Warn_vector.clear();
    vt.WarnSign(srcImage,Warn_vector);
    pthread_exit(0);
}
void *sign(void*args){// 禁令标志线程
    Mat src=srcImage.clone();
    signprepframe = signpreprocessor.getCurrentPreprocessedImage(src);
    sign_rect_region=sd.signDetector(signprepframe,srcImage);
    pthread_exit(0);
}
int main()
{
    m=0;
    Mat m_slowdown_Mask;
    Mat m_slowdown=imread("slowdown.png");
    resize(m_slowdown,m_slowdown,Size(120,120));
    cvtColor(m_slowdown,m_slowdown_Mask,CV_RGB2GRAY);
    Mat m_people_Mask;
    Mat m_people=imread("people.jpg");
    resize(m_people,m_people,Size(120,120));
    cvtColor(m_people,m_people_Mask,CV_RGB2GRAY);
    Mat m_child_Mask;
    Mat m_child=imread("child.jpg");
    resize(m_child,m_child,Size(120,120));
    cvtColor(m_child,m_child_Mask,CV_RGB2GRAY);
    Mat m_construction_Mask;
    Mat m_construction=imread("construction.png");
    resize(m_construction,m_construction,Size(120,120));
    cvtColor(m_construction,m_construction_Mask,CV_RGB2GRAY);
   // number=1;
    //file.open("/home/lee/桌面/warn_label/test_warn2.txt", ofstream::out);
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
    namedWindow("Video");
    VideoCapture cap("/home/lee/桌面/TSR/警告标志/视频/HPIM0016.mov");// 打开视频文件
    if (!cap.isOpened())
    {
        cout << "读取视频有误" << endl;
        return -1;
    }

    while (!stop)
    {
        t=getTickCount();// 统计时间戳
        cap>>srcImage;
        rectangle(srcImage,Rect(550,660,180,60),Scalar(255,255,255));
        pthread_t tids[3];
        //thread 0
        int ret = pthread_create( &tids[0], &attr, function_light, NULL );// 信号灯
        if(ret!=0){
            cout<<"pthread_join error:error_code="<<endl;
        }
       pthread_join(tids[0],NULL);
        //thread 1
        int ret1 = pthread_create( &tids[1], &attr, warning, NULL );// 警告标志
        if(ret1!=0){
            cout<<"pthread_join error:error_code="<<endl;
        }
        pthread_join(tids[1],NULL);
        //thread 2
        int ret2 = pthread_create( &tids[2], &attr, sign, NULL );// 禁令标志
        if(ret2!=0){
            cout<<"pthread_join error:error_code="<<endl;
        }
        pthread_join(tids[2],NULL);
/* Writer light information */
        if(Light_vector.size()>0)
        {
            //file<<number<<" "<<Light_vector.size()<<endl;
            for(int i=0;i<Light_vector.size();i++)
            {
//                                file<<number<<" "<<Light_vector[i].m_Rect.x<<" "<<Light_vector[i].m_Rect.y<<" "<<Light_vector[i].m_Rect.width<<
//                                      " "<<Light_vector[i].m_Rect.height<<" "<<"light"<<endl;
                rectangle(srcImage,Light_vector[i].m_Rect,Light_vector[i].m_color,2);
                putText(srcImage,Light_vector[i].m_meaning,Point(Light_vector[i].m_Rect.x-2,Light_vector[i].m_Rect.y-5),
                        FONT_HERSHEY_SIMPLEX, 0.6, Light_vector[i].m_color, 2);
            }
               //number++;
        }
//                else
//                {
//                    file<<number<<" "<<Light_vector.size()<<endl;
//                     number++;
//                }
        switch (Light_vector.size())
        {
        case 1:
            circle(srcImage,Point(640,690),20,Light_vector[0].m_color,-1,8);
            circle(srcImage,Point(580,690),20,Scalar(0,0,0),-1,8);
            circle(srcImage,Point(700,690),20,Scalar(0,0,0),-1,8);
            break;
        case 2:
            circle(srcImage,Point(580,690),20,Light_vector[1].m_color,-1,8);
            circle(srcImage,Point(640,690),20,Light_vector[0].m_color,-1,8);
            circle(srcImage,Point(700,690),20,Scalar(0,0,0),-1,8);
            break;
        case 3:
            circle(srcImage,Point(580,690),20,Light_vector[2].m_color,-1,8);
            circle(srcImage,Point(640,690),20,Light_vector[1].m_color,-1,8);
            circle(srcImage,Point(700,690),20,Light_vector[0].m_color,-1,8);
            break;
        default:
            circle(srcImage,Point(580,690),20,Scalar(0,0,0),-1,8);
            circle(srcImage,Point(640,690),20,Scalar(0,0,0),-1,8);
            circle(srcImage,Point(700,690),20,Scalar(0,0,0),-1,8);
            break;
        }
/* Writer warn information */
        if(Warn_vector.size()>0)
        {
            Mat imageROI=srcImage(Rect(430,600,120,120));
            //file<<number<<" "<<Warn_vector.size()<<endl;
            for(int i=0;i<Warn_vector.size();i++)
            {
//                file<<number<<" "<<Warn_vector[i].m_warnRect.x<<" "<<Warn_vector[i].m_warnRect.y<<" "<<Warn_vector[i].m_warnRect.width<<
//                      " "<<Warn_vector[i].m_warnRect.height<<" "<<"warn"<<endl;
                switch (Warn_vector[i].m_label) {
                case 1:
                    m_slowdown.copyTo(imageROI,m_slowdown_Mask);
                    break;
                case 2:
                     m_people.copyTo(imageROI,m_people_Mask);
                    break;
                case 3:
                     m_child.copyTo(imageROI,m_child_Mask);
                    break;
                case 4:
                     m_construction.copyTo(imageROI,m_construction_Mask);
                    break;
                default:
                    break;
                }
                putText(srcImage,Warn_vector[i].m_warnMeaning,Point(Warn_vector[i].m_warnRect.x-2,Warn_vector[i].m_warnRect.y-5),
                        FONT_HERSHEY_SIMPLEX, 0.6,Scalar(0,255,255), 2);
                rectangle(srcImage,Warn_vector[i].m_warnRect,Scalar(0,255,255),2);
            }
           //number++;
        }
//        else
//        {
//            file<<number<<" "<<Warn_vector.size()<<endl;
//             number++;
//        }
/* Writer prohibition information */
        if(sign_rect_region.size()>0){
            for(auto c:sign_rect_region){
                if (c._traffic_sign == 0){
                    putText(srcImage, "rotation", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                }
                if (c._traffic_sign == 1){
                    putText(srcImage, "turn right", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                }
                if (c._traffic_sign == 2){
                    putText(srcImage, "in", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                }
                if (c._traffic_sign == 10){
                    putText(srcImage, "parking", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                }
                if (c._traffic_sign == 7){
                    putText(srcImage, "honk", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                }
                if (c._traffic_sign == 6){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "onevehicle enter", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 8){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "truck", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 9){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "buses", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 3){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "40", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 4){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "50", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 5){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "60", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign==11){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage,"left",Point(c._start_point_x,c._start_point_y),1,1,CV_RGB(255,0,0),2);
                }
                if (c._traffic_sign == 12){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "30", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 13){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "10", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 14){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "70", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 15){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "80", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 16){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "90", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 17){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "25", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 18){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "35", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 19){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "55", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 20){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "65", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 21){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "75", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 22){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "85", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 23){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "95", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 24){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "20", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 25){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "5", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign ==26){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "100", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 27){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "120", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 28){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "110", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
                if (c._traffic_sign == 29){
                    rectangle(srcImage,Rect(c._start_point_x, c._start_point_y,c._width,c._height),Scalar(0,0,255),2);
                    putText(srcImage, "15", Point(c._start_point_x, c._start_point_y), 1, 1, CV_RGB(255, 0, 0), 2);
                }
            }
        }
/*##############################*/
        imshow("Video",srcImage);
//        char name[200];
//        sprintf(name, "%s%.6d%s", "/media/lee/Lee-Disk/TSR文件夹/Video/LaneSky/", ++m, ".jpg");//保存的图片名
//        imwrite(name,srcImage);
        t=(getTickCount()-t)/getTickFrequency();// 计算每一帧运行时间
        fps=1/t;//得到帧率
        cout<<"fps: "<<fps<<endl;
        int key = waitKey(30);
        if ( 's' == char(key))
        {
            stop = true;
        }
        else if('w'== char(key))
        {
            int current_rate = rate_base*rate;
            while(current_rate)
            {
                cap>>srcImage;
                current_rate--;
            }
        }
        else if('r'==char(key))
        {
            if(rate_base<=1)
            {
                rate_base = 5;
            }
            else
            {
                rate_base--;
            }
            cout<<"Rate: "<<rate_base<<" * "<<rate<<endl;
        }
        else if('p'==char(key))
        {
            pause = !pause;
        }
    }
    return 0;
}
