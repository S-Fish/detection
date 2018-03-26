#ifndef HISTOGRAM1D_H_
#define HISTOGRAM1D_H_

#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<iostream>
using namespace std;

class Histogram1D{
private:
	int histSize[1];//项的数量
	float hranges[2];//像素的最小和最大值
	const float* ranges[1];
	int channels[1];//仅用到一个通道

public:
	Histogram1D();
	//计算1D直方图
	cv::MatND getHistogram(const cv::Mat &image);

	cv::Mat getHistogramImage(const cv::Mat &image);

	cv::Mat applyLookUp(
		const cv::Mat& image,	//输入图像
		const cv::Mat& lookup	//输出图像
		);

	//图片拉伸
	cv::Mat stretch(const cv::Mat &image, int minValue);

	cv::Mat applyLookUp(cv::Mat &image, const cv::Mat& lookup);
};

#endif