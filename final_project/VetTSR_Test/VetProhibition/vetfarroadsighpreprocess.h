#ifndef VETFARROADSIGNPREPROCESSOR_H
#define VETFARROADSIGNPREPROCESSOR_H
#include"struct.h"
class VetFarRoadSignPreprocessor
{
public:
	VetFarRoadSignPreprocessor();
	~VetFarRoadSignPreprocessor();

public:
    Mat getCurrentPreprocessedImage(Mat &);  //ͨ����������ԭʼͼ�񣬽���ͼ��Ԥ��������洢��List��β�����������
    PAIR_T_M getLastPreprocessedImage();  //�������µ�ͼ��

private:
	void rectImage(Mat&);   //ROI��ȡ
	void changeMode(Mat&);  //��RGB�ռ�ת����HSV�ռ�
	void binaryImage(Mat&);  //��ֵ����
	void gaussianblur(Mat&);  //��˹ƽ��
	void erodeImage(Mat&);  //��ʴ
	void dilateImage(Mat&);  //����
private:
	Mat norm_0_255(const Mat&);
	void on_mouse(int event, int x, int y, int flags, void *ustc);
	void gammaCorrection(Mat&);

private:

	Mat _image_data;

};

#endif // VETFARROADSIGNPREPROCESSOR_H
