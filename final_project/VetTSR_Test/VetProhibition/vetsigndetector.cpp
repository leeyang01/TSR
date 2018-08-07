#include"vetsigndetector.h"
#include"vetsvmhog.h"
#include"struct.h"
#include"vetlightpreprocessor.h"
VetSignDetector::VetSignDetector(){
	//对SVM模型进行加载
    svm_red_second.load("second_red_new_addnopark_auto.xml");
    svm_red_first.load("first_red_new_addnopark_auto.xml");
    svm_digital.load("digital_color_auto.xml");
    svm_circle.load("circle_detect.xml");
	i_num_region_sign = 0;
}

VetSignDetector::~VetSignDetector(){

}

vector<RectRegion> VetSignDetector::signDetector(Mat& imagedata,Mat& initial_data){
	rect_region_sign.clear();
	rect_region_number.clear();
	rect_region.clear();
	getSignCircle(imagedata,initial_data);
	recogSign(initial_data);
    return rect_region;
}

void VetSignDetector::getSignCircle(Mat &imagedata,Mat &initial_data){
	CvRect r;
    vector<vector<Point>> contours;
    vector<Vec4i> hierarchy;
    Mat _img = imagedata.clone();
    findContours(_img, contours, hierarchy, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point(0, 0));    for (int i=0;i<contours.size();i++)
	{
		//获取轮廓面积
        double contArea = fabs(contourArea(contours[i],true));
		if (contArea >= 100){
			//获取矩形轮廓

            r=boundingRect(Mat(contours[i]));
			Mat a(imagedata, Rect(r.x, r.y, r.width, r.height));
			float c_result = 0;
			vector<float> vec_descriptors;// HOG描述子向量，每个连通区域的梯度直方图
			int i_DescriptorDim, i;
			//标志牌HOG特征
			HOGDescriptor hod_hog(Size(32, 32), Size(16, 16), Size(8, 8), Size(8, 8), 9);//检测窗口大小，块大小，块步长，胞元大小，梯度方向数

			Mat mat_test_feature; // 所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数	
			//Mat mat_test_label; // 训练样本的类别向量，行数等于所有样本的个数，列数等于1；

			//将候选区域resize到窗口大小。
			resize(a, a, Size(32, 32));
			//imshow("s", imagedata);
			//waitKey(0);
			//计算候选区域HOG描述子，检测窗口移动步长为（0,0）,因为候选区域已经resize到窗口大小，窗口不需要进行滑动。
			hod_hog.compute(a, vec_descriptors, Size(0, 0));

			// HOG描述子的维数
			i_DescriptorDim = vec_descriptors.size();
            // << "维数：" << i_DescriptorDim << endl;
			// 初始化所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数.
			mat_test_feature = Mat::zeros(1, i_DescriptorDim, CV_32FC1); // bit_depth:32 F:float 1:channel

			// 将计算好的HOG描述子复制到样本特征矩阵mat_test_feature
			for (i = 0; i < i_DescriptorDim; i++)
				mat_test_feature.at<float>(0, i) = vec_descriptors[i];
			c_result = svm_circle.predict(mat_test_feature);
			CvRect r_one, r_two_1, r_two_2, r_three_1, r_three_2, r_three_3;
			if (c_result == 1)
			{
                //cout << "    circle" << endl;
				string str = "circle";
				r_one.x = ((r.x + (initial_data.cols * 1 / 2) * 1 / 3)) * 2;
				r_one.y = r.y * 2;
				r_one.width = r.width * 2;
				r_one.height = r.height * 2;
				rect_region_sign.push_back(r_one);

				putText(imagedata, str, Point(r.x + r.width / 2, r.y + r.height / 2),
					CV_FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 1);
			}
			else if (c_result == 2)
			{
                //cout << "  two  circle" << endl;
				string str = "two circle";

				if (r.width / r.height >= 1.0)
				{
					r_two_1.x = ((r.x + (initial_data.cols * 1 / 2) * 1 / 3)) * 2;
					r_two_1.y = r.y * 2;
					r_two_1.width = r.width;
					r_two_1.height = r.height * 2;

					r_two_2.x = ((r.x + (initial_data.cols * 1 / 2) * 1 / 3)) * 2 + r.width;
					r_two_2.y = r.y * 2;
					r_two_2.width = r.width;
					r_two_2.height = r.height * 2;

					rect_region_sign.push_back(r_two_1);
					rect_region_sign.push_back(r_two_2);

				}
				else if (r.width / r.height <= 0.8)
				{
					r_two_1.x = ((r.x + (initial_data.cols * 1 / 2) * 1 / 3)) * 2;
					r_two_1.y = r.y * 2;
					r_two_1.width = r.width * 2;
					r_two_1.height = r.height;

					r_two_2.x = r.x * 2;
					r_two_2.y = r.y * 2 + r.height;
					r_two_2.width = r.width * 2;
					r_two_2.height = r.height;

					rect_region_sign.push_back(r_two_1);
					rect_region_sign.push_back(r_two_2);
				}
				putText(imagedata, str, Point(r.x + r.width / 2, r.y + r.height / 2),
					CV_FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 1);

			}
			//三个交通标志连在一块的情况目前还未做处理
			else if (c_result == 3)
			{
				if (r.width / r.height >= 2.0){
					r_three_1.x = ((r.x + (initial_data.cols * 1 / 2) * 1 / 3)) * 2;
					r_three_1.y = r.y * 2;
					r_three_1.width = r.width*2/3;
					r_three_1.height = r.height * 2;

					r_three_2.x = ((r.x + (initial_data.cols * 1 / 2) * 1 / 3)) * 2 + r.width*2/3;
					r_three_2.y = r.y * 2;
					r_three_2.width = r.width*2/3;
					r_three_2.height = r.height * 2;

					r_three_3.x = ((r.x + (initial_data.cols * 1 / 2) * 1 / 3)) * 2 + r.width*4/3;
					r_three_3.y = r.y * 2;
					r_three_3.width = r.width*2/3;
					r_three_3.height = r.height * 2;

					rect_region_sign.push_back(r_three_1);
					rect_region_sign.push_back(r_three_2);
					rect_region_sign.push_back(r_three_3);

				}
			
                //cout << "  three  circle" << endl;
				string str = "three circle";
				putText(imagedata, str, Point(r.x + r.width / 2, r.y + r.height / 2),
					CV_FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1);
			}
		}
	}
    //return imagedata;
}

//四舍五入
double VetSignDetector::round(double r)
{
	return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

/*!
* \brief VetSignStrategyHogTemp::recogSign 标志牌候选区域识别模块
*/
void VetSignDetector::recogSign(Mat& initial_data)
{
	int i;
	float f_result;
	Mat mat_region;

	for (i = 0; i < rect_region_sign.size(); i++)
	{

		mat_region = initial_data(rect_region_sign[i]);
		//调用recogPicture（）进行分类
		f_result = recogPicture(mat_region);

		//禁止回转
		if (f_result == 1)
		{
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::turn });
		}

		//禁止右转
		else if (f_result == 2)
		{
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::right });
		}

		//禁止载货车辆驶入
		else if (f_result == 3)
		{
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::goods });
		}

		//禁止通行
		else if (f_result == 4)
		{
			
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::in });
			
		}

		//禁止大型客车驶入
		else if (f_result == 6)
		{
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::bus });
			
		}

		//禁鸣喇叭
		else if (f_result == 7)
		{
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::honk });
		}

		//禁止该种车型驶入
		else if (f_result == 8)
		{
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::onevehicle });
		}

		//禁止停车
		else if (f_result == 9)
		{
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::park });
		}

		// 限速标志，由于测试视频中只有限速40、50、60，所以以下只对这三类进行了处理，实际该算法可以识别出所有限速标志。
		else if (f_result != 0)
		{
			// 限速50
			if (f_result == 50)
			{
				rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::speed_50 });
			}

			// 限速40
			if (f_result == 40)
			{
				rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::speed_40 });
			}

			// 限速60
			if (f_result == 60)
			{
				rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::speed_60 });
			}
		}
	}
}

float VetSignDetector::recogPicture(Mat& imagedata){
	float f_result=0;
	vector<float> vec_descriptors;// HOG描述子向量
	int i_DescriptorDim, i;
	//标志牌HOG特征
	HOGDescriptor hod_hog(Size(VET_RED_WIDTH, VET_RED_HEIGHT), Size(16, 16), Size(8, 8), Size(8, 8), 9);

	Mat mat_test_feature; // 所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数	
	//Mat mat_test_label; // 训练样本的类别向量，行数等于所有样本的个数，列数等于1；

	//将候选区域resize到窗口大小。
	resize(imagedata, imagedata, Size(VET_RED_WIDTH, VET_RED_HEIGHT));
	/*imshow("s", imagedata);
	waitKey(0);*/
	//计算候选区域HOG描述子，检测窗口移动步长为（0,0）,因为候选区域已经resize到窗口大小，窗口不需要进行滑动。
	hod_hog.compute(imagedata, vec_descriptors, Size(0, 0));
	// HOG描述子的维数
	i_DescriptorDim = vec_descriptors.size();

	// 初始化所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数.
	mat_test_feature = Mat::zeros(1, i_DescriptorDim, CV_32FC1); // bit_depth:32 F:float 1:channel

	// 将计算好的HOG描述子复制到样本特征矩阵mat_test_feature
	for (i = 0; i < i_DescriptorDim; i++)
		mat_test_feature.at<float>(0, i) = vec_descriptors[i];

	f_result = svm_red_first.predict(mat_test_feature);   //第一级标志牌识别 0（非） 1（普通） 2（限速）
	
	if (f_result == 1)	//将普通标志牌送入第二级分类器中进行具体语义分类  
	{
		f_result = svm_red_second.predict(mat_test_feature);
		
	}

	else if (f_result == 2) //若是限速标志，则将其放入sign_region容器中
	{
		f_result = detectNumber(imagedata);
		//f_result = sign.detectNumber(imagedata);
	}
	return f_result;
}

int VetSignDetector::detectNumber(Mat& imagedata){
	//放大限速标志牌，便于后续的数字分割

	resize(imagedata, imagedata, Size(150, 150));
	//imshow("imagedata", imagedata);
	//提取数字区域
	Mat mat_deal_img = imagedata.clone();   //后续处理
	Mat mat_draw_img = mat_deal_img.clone();   //画框	
	Mat mat_save_img = mat_deal_img.clone();   //提取区域后保存

	//灰度化
	cvtColor(mat_deal_img, mat_deal_img, CV_BGR2GRAY);

	//OTSU二值化
	int i_threshold = getGrayThre(mat_deal_img);
	threshold(mat_deal_img, mat_deal_img, i_threshold, 255, 1);

	//开运算
	Mat element = getStructuringElement(MORPH_RECT, Size(2, 2));
	morphologyEx(mat_deal_img, mat_deal_img, MORPH_OPEN, element);

	//连通区域轮廓提取
	finddigitalContours(mat_deal_img, mat_draw_img);
	//识别数字得到具体限速
	int i_digi = recogNumber(imagedata);

	return  i_digi;
}

int VetSignDetector::getGrayThre(Mat imagedata){
	int height = imagedata.rows;
	int width = imagedata.cols;

	//histogram        
	float histogram[256] = { 0 };
	for (int i = 0; i < height; i++)
	{
		unsigned char* p = (unsigned char*)imagedata.ptr<uchar>(i);
		for (int j = 0; j < width; j++)
		{
			histogram[p[j]]++;
		}
	}
	//normalize histogram        
	int size = height * width;
	for (int i = 0; i < 256; i++)
	{
		histogram[i] = histogram[i] / size;
	}

	//average pixel value        
	float avgValue = 0;
	for (int i = 0; i < 256; i++)
	{
		avgValue += i * histogram[i];
	}

	int threshold;
	float maxVariance = 0;
	float w = 0, u = 0;
	for (int i = 0; i < 256; i++)
	{
		w += histogram[i];  //假设当前灰度i为阈值, 0~i 灰度的像素(假设像素值在此范围的像素叫做前景像素) 所占整幅图像的比例      
		u += i * histogram[i];  // 灰度i之前的像素(0~i)的平均灰度值： 前景像素的平均灰度值      

		float t = avgValue * w - u;
		float variance = t * t / (w * (1 - w));
		if (variance > maxVariance)
		{
			maxVariance = variance;
			threshold = i;
		}
	}
	return threshold;
}

void VetSignDetector::finddigitalContours(Mat& imagedata, Mat& draw_imagedata){
    CvRect rec;
    vector<vector<Point>> contours;
    vector<Vec4i>hierarchy;
    Mat _img = imagedata.clone();
    blur(imagedata, imagedata, Size(3, 3));
    findContours(_img, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE, Point());
    for (int i = 0; i < contours.size(); i++)
    {
       //获取轮廓面积
       double contArea = fabs(contourArea(contours[i], true));
       //获取矩形轮廓
       rec = boundingRect(Mat(contours[i]));
       if (contArea > 1000 && contArea < 6000)  //之前把标志牌resize到了（150,150），所以筛选面积要增大。
        {
            rectangle(draw_imagedata, rec, CV_RGB(255, 0, 0));  //测试时框出数字区域
            rect_region_number.push_back(rec);
         }
     }
}

int VetSignDetector::recogNumber(Mat& imagedata){
	bool is_zero = false, is_five = false, is_other = false;
	int i_num_zero = 0, i_num_one = 0, i_num_two = 0, other_digital = 0, i_num_digital = 0;

	//数字HOG特征尺寸
	HOGDescriptor hog(Size(20, 20), Size(10, 10), Size(5, 5), Size(5, 5), 9);
	vector< float > vec_descriptors;
	int i_DescriptorDim;
	float f_result;
	int i, j;
	for (i = 0; i < rect_region_number.size(); i++)
	{
		//获取数字候选区域
		Mat mat_digital = imagedata(rect_region_number[i]);

		//resize至窗口大小
		resize(mat_digital, mat_digital, Size(VET_DIGITAL_WIDTH, VET_DIGITAL_HEIGHT));

		//提取候选区域的hog特征
		hog.compute(mat_digital, vec_descriptors, Size(8, 8));

		// HOG描述子的维数
		i_DescriptorDim = vec_descriptors.size();

		// 初始化所有训练样本的特征向量组成的矩阵，行数等于所有样本的个数，列数等于HOG描述子维数.
		Mat mat_test_feature = Mat::zeros(1, i_DescriptorDim, CV_32FC1); // bit_depth:32 F:float 1:channel

		// 将计算好的HOG描述子复制到样本特征矩阵mat_test_feature
		for (j = 0; j < i_DescriptorDim; j++)
			mat_test_feature.at< float >(0, j) = vec_descriptors[j];

		//分类
		f_result = svm_digital.predict(mat_test_feature);

		//若数字候选区域数大于等于3，则需要额外判断是否是限速100,110,120；
		if (rect_region_number.size() >= 3)
		{
			if (f_result == 0)
			{
				i_num_zero++;
				if (i_num_zero == 2 && i_num_one == 1)
					return 100;
				else if (i_num_zero == 1 && i_num_one == 2)
					return 110;
				else if (i_num_zero == 1 && i_num_one == 1 && i_num_two == 1)
					return 120;
			}
			else if (f_result == 1)
			{
				i_num_one++;
				if (i_num_zero == 2 && i_num_one == 1)
					return 100;
				else if (i_num_zero == 1 && i_num_one == 2)
					return 110;
				else if (i_num_zero == 1 && i_num_one == 1 && i_num_two == 1)
					return 120;
			}

			else if (f_result == 2)
			{
				i_num_two++;
				if (i_num_zero == 1 && i_num_one == 1 && i_num_two == 1)
					return 120;
			}
		}

		//识别5-95的限速标志
		if (f_result == 0)
		{
			i_num_digital++;
			is_zero = true;
			if (is_five == true)
				return 50;
			else if (is_other == true)
				return other_digital * 10;
		}

		else if (f_result == 5)
		{
			i_num_digital++;
			is_five = true;
			if (is_zero == true)
				return 50;
			else if (is_other == true)
				return other_digital * 10 + 5;
		}

		else if (f_result != 5 && f_result != 0 && f_result != 10)
		{
			i_num_digital++;
			is_other = true;
			other_digital = f_result;
			if (is_zero == true)
				return other_digital * 10;

			else if (is_five == true)
				return other_digital * 10 + 5;
		}
	}
	if (is_five == true && i_num_digital == 1)
		return 5;

	//非限速标志
	else
		return 0;
}
