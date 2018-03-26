#ifndef HISTOGRAM1D_H_
#define HISTOGRAM1D_H_

#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<iostream>
using namespace std;

class Histogram1D{
private:
	int histSize[1];//�������
	float hranges[2];//���ص���С�����ֵ
	const float* ranges[1];
	int channels[1];//���õ�һ��ͨ��

public:
	Histogram1D();
	//����1Dֱ��ͼ
	cv::MatND getHistogram(const cv::Mat &image);

	cv::Mat getHistogramImage(const cv::Mat &image);

	cv::Mat applyLookUp(
		const cv::Mat& image,	//����ͼ��
		const cv::Mat& lookup	//���ͼ��
		);

	//ͼƬ����
	cv::Mat stretch(const cv::Mat &image, int minValue);

	cv::Mat applyLookUp(cv::Mat &image, const cv::Mat& lookup);
};

#endif