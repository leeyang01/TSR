#include"vetsigndetector.h"
#include"vetsvmhog.h"
#include"struct.h"
#include"vetlightpreprocessor.h"
VetSignDetector::VetSignDetector(){
	//��SVMģ�ͽ��м���
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
		//��ȡ�������
        double contArea = fabs(contourArea(contours[i],true));
		if (contArea >= 100){
			//��ȡ��������

            r=boundingRect(Mat(contours[i]));
			Mat a(imagedata, Rect(r.x, r.y, r.width, r.height));
			float c_result = 0;
			vector<float> vec_descriptors;// HOG������������ÿ����ͨ������ݶ�ֱ��ͼ
			int i_DescriptorDim, i;
			//��־��HOG����
			HOGDescriptor hod_hog(Size(32, 32), Size(16, 16), Size(8, 8), Size(8, 8), 9);//��ⴰ�ڴ�С�����С���鲽������Ԫ��С���ݶȷ�����

			Mat mat_test_feature; // ����ѵ������������������ɵľ��������������������ĸ�������������HOG������ά��	
			//Mat mat_test_label; // ѵ����������������������������������ĸ�������������1��

			//����ѡ����resize�����ڴ�С��
			resize(a, a, Size(32, 32));
			//imshow("s", imagedata);
			//waitKey(0);
			//�����ѡ����HOG�����ӣ���ⴰ���ƶ�����Ϊ��0,0��,��Ϊ��ѡ�����Ѿ�resize�����ڴ�С�����ڲ���Ҫ���л�����
			hod_hog.compute(a, vec_descriptors, Size(0, 0));

			// HOG�����ӵ�ά��
			i_DescriptorDim = vec_descriptors.size();
            // << "ά����" << i_DescriptorDim << endl;
			// ��ʼ������ѵ������������������ɵľ��������������������ĸ�������������HOG������ά��.
			mat_test_feature = Mat::zeros(1, i_DescriptorDim, CV_32FC1); // bit_depth:32 F:float 1:channel

			// ������õ�HOG�����Ӹ��Ƶ�������������mat_test_feature
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
			//������ͨ��־����һ������Ŀǰ��δ������
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

//��������
double VetSignDetector::round(double r)
{
	return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

/*!
* \brief VetSignStrategyHogTemp::recogSign ��־�ƺ�ѡ����ʶ��ģ��
*/
void VetSignDetector::recogSign(Mat& initial_data)
{
	int i;
	float f_result;
	Mat mat_region;

	for (i = 0; i < rect_region_sign.size(); i++)
	{

		mat_region = initial_data(rect_region_sign[i]);
		//����recogPicture�������з���
		f_result = recogPicture(mat_region);

		//��ֹ��ת
		if (f_result == 1)
		{
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::turn });
		}

		//��ֹ��ת
		else if (f_result == 2)
		{
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::right });
		}

		//��ֹ�ػ�����ʻ��
		else if (f_result == 3)
		{
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::goods });
		}

		//��ֹͨ��
		else if (f_result == 4)
		{
			
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::in });
			
		}

		//��ֹ���Ϳͳ�ʻ��
		else if (f_result == 6)
		{
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::bus });
			
		}

		//��������
		else if (f_result == 7)
		{
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::honk });
		}

		//��ֹ���ֳ���ʻ��
		else if (f_result == 8)
		{
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::onevehicle });
		}

		//��ֹͣ��
		else if (f_result == 9)
		{
			rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::park });
		}

		// ���ٱ�־�����ڲ�����Ƶ��ֻ������40��50��60����������ֻ������������˴���ʵ�ʸ��㷨����ʶ����������ٱ�־��
		else if (f_result != 0)
		{
			// ����50
			if (f_result == 50)
			{
				rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::speed_50 });
			}

			// ����40
			if (f_result == 40)
			{
				rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::speed_40 });
			}

			// ����60
			if (f_result == 60)
			{
				rect_region.push_back({ rect_region_sign[i].x, rect_region_sign[i].y, rect_region_sign[i].width, rect_region_sign[i].height, Result::speed_60 });
			}
		}
	}
}

float VetSignDetector::recogPicture(Mat& imagedata){
	float f_result=0;
	vector<float> vec_descriptors;// HOG����������
	int i_DescriptorDim, i;
	//��־��HOG����
	HOGDescriptor hod_hog(Size(VET_RED_WIDTH, VET_RED_HEIGHT), Size(16, 16), Size(8, 8), Size(8, 8), 9);

	Mat mat_test_feature; // ����ѵ������������������ɵľ��������������������ĸ�������������HOG������ά��	
	//Mat mat_test_label; // ѵ����������������������������������ĸ�������������1��

	//����ѡ����resize�����ڴ�С��
	resize(imagedata, imagedata, Size(VET_RED_WIDTH, VET_RED_HEIGHT));
	/*imshow("s", imagedata);
	waitKey(0);*/
	//�����ѡ����HOG�����ӣ���ⴰ���ƶ�����Ϊ��0,0��,��Ϊ��ѡ�����Ѿ�resize�����ڴ�С�����ڲ���Ҫ���л�����
	hod_hog.compute(imagedata, vec_descriptors, Size(0, 0));
	// HOG�����ӵ�ά��
	i_DescriptorDim = vec_descriptors.size();

	// ��ʼ������ѵ������������������ɵľ��������������������ĸ�������������HOG������ά��.
	mat_test_feature = Mat::zeros(1, i_DescriptorDim, CV_32FC1); // bit_depth:32 F:float 1:channel

	// ������õ�HOG�����Ӹ��Ƶ�������������mat_test_feature
	for (i = 0; i < i_DescriptorDim; i++)
		mat_test_feature.at<float>(0, i) = vec_descriptors[i];

	f_result = svm_red_first.predict(mat_test_feature);   //��һ����־��ʶ�� 0���ǣ� 1����ͨ�� 2�����٣�
	
	if (f_result == 1)	//����ͨ��־������ڶ����������н��о����������  
	{
		f_result = svm_red_second.predict(mat_test_feature);
		
	}

	else if (f_result == 2) //�������ٱ�־���������sign_region������
	{
		f_result = detectNumber(imagedata);
		//f_result = sign.detectNumber(imagedata);
	}
	return f_result;
}

int VetSignDetector::detectNumber(Mat& imagedata){
	//�Ŵ����ٱ�־�ƣ����ں��������ַָ�

	resize(imagedata, imagedata, Size(150, 150));
	//imshow("imagedata", imagedata);
	//��ȡ��������
	Mat mat_deal_img = imagedata.clone();   //��������
	Mat mat_draw_img = mat_deal_img.clone();   //����	
	Mat mat_save_img = mat_deal_img.clone();   //��ȡ����󱣴�

	//�ҶȻ�
	cvtColor(mat_deal_img, mat_deal_img, CV_BGR2GRAY);

	//OTSU��ֵ��
	int i_threshold = getGrayThre(mat_deal_img);
	threshold(mat_deal_img, mat_deal_img, i_threshold, 255, 1);

	//������
	Mat element = getStructuringElement(MORPH_RECT, Size(2, 2));
	morphologyEx(mat_deal_img, mat_deal_img, MORPH_OPEN, element);

	//��ͨ����������ȡ
	finddigitalContours(mat_deal_img, mat_draw_img);
	//ʶ�����ֵõ���������
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
		w += histogram[i];  //���赱ǰ�Ҷ�iΪ��ֵ, 0~i �Ҷȵ�����(��������ֵ�ڴ˷�Χ�����ؽ���ǰ������) ��ռ����ͼ��ı���      
		u += i * histogram[i];  // �Ҷ�i֮ǰ������(0~i)��ƽ���Ҷ�ֵ�� ǰ�����ص�ƽ���Ҷ�ֵ      

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
       //��ȡ�������
       double contArea = fabs(contourArea(contours[i], true));
       //��ȡ��������
       rec = boundingRect(Mat(contours[i]));
       if (contArea > 1000 && contArea < 6000)  //֮ǰ�ѱ�־��resize���ˣ�150,150��������ɸѡ���Ҫ����
        {
            rectangle(draw_imagedata, rec, CV_RGB(255, 0, 0));  //����ʱ�����������
            rect_region_number.push_back(rec);
         }
     }
}

int VetSignDetector::recogNumber(Mat& imagedata){
	bool is_zero = false, is_five = false, is_other = false;
	int i_num_zero = 0, i_num_one = 0, i_num_two = 0, other_digital = 0, i_num_digital = 0;

	//����HOG�����ߴ�
	HOGDescriptor hog(Size(20, 20), Size(10, 10), Size(5, 5), Size(5, 5), 9);
	vector< float > vec_descriptors;
	int i_DescriptorDim;
	float f_result;
	int i, j;
	for (i = 0; i < rect_region_number.size(); i++)
	{
		//��ȡ���ֺ�ѡ����
		Mat mat_digital = imagedata(rect_region_number[i]);

		//resize�����ڴ�С
		resize(mat_digital, mat_digital, Size(VET_DIGITAL_WIDTH, VET_DIGITAL_HEIGHT));

		//��ȡ��ѡ�����hog����
		hog.compute(mat_digital, vec_descriptors, Size(8, 8));

		// HOG�����ӵ�ά��
		i_DescriptorDim = vec_descriptors.size();

		// ��ʼ������ѵ������������������ɵľ��������������������ĸ�������������HOG������ά��.
		Mat mat_test_feature = Mat::zeros(1, i_DescriptorDim, CV_32FC1); // bit_depth:32 F:float 1:channel

		// ������õ�HOG�����Ӹ��Ƶ�������������mat_test_feature
		for (j = 0; j < i_DescriptorDim; j++)
			mat_test_feature.at< float >(0, j) = vec_descriptors[j];

		//����
		f_result = svm_digital.predict(mat_test_feature);

		//�����ֺ�ѡ���������ڵ���3������Ҫ�����ж��Ƿ�������100,110,120��
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

		//ʶ��5-95�����ٱ�־
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

	//�����ٱ�־
	else
		return 0;
}
