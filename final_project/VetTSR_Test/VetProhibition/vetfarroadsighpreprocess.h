#ifndef VETFARROADSIGNPREPROCESSOR_H
#define VETFARROADSIGNPREPROCESSOR_H
#include"struct.h"
class VetFarRoadSignPreprocessor
{
public:
	VetFarRoadSignPreprocessor();
	~VetFarRoadSignPreprocessor();

public:
    Mat getCurrentPreprocessedImage(Mat &);  //通过参数输入原始图像，进行图像预处理，结果存储在List对尾，并对外输出
    PAIR_T_M getLastPreprocessedImage();  //返回最新的图像

private:
	void rectImage(Mat&);   //ROI截取
	void changeMode(Mat&);  //从RGB空间转换到HSV空间
	void binaryImage(Mat&);  //二值化；
	void gaussianblur(Mat&);  //高斯平滑
	void erodeImage(Mat&);  //腐蚀
	void dilateImage(Mat&);  //膨胀
private:
	Mat norm_0_255(const Mat&);
	void on_mouse(int event, int x, int y, int flags, void *ustc);
	void gammaCorrection(Mat&);

private:

	Mat _image_data;

};

#endif // VETFARROADSIGNPREPROCESSOR_H
