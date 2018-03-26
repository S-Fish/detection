#include"histogram1D.h"



Histogram1D::Histogram1D(){
	this->histSize[0] = 256;
	this->hranges[0] = 0.0;
	this->hranges[1] = 255.0;
	ranges[0] = this->hranges;
	channels[0] = 0;//考察0通道
}
//计算1D直方图
cv::MatND Histogram1D::getHistogram(const cv::Mat &image){
	cv::MatND hist;

	//计算直方图，调用cv::库函数就行
	cv::calcHist(&image,
		1,				//计算单张图像的直方图
		channels,		//通道数量
		cv::Mat(),		//不使用图像作为掩码
		hist,			//返回的是直方图
		1,				//这是1D的直方图（单像素）
		histSize,		//项的数量	
		ranges			//像素值的范围
		);
	return hist;
}

cv::Mat Histogram1D::getHistogramImage(const cv::Mat &image){

	//计算直方图
	cv::MatND hist = getHistogram(image);

	//获得最大和最小值
	double maxVal = 0;
	double minVal = 0;
	cv::minMaxLoc(hist, &minVal, &maxVal, 0, 0);

	//显示图
	cv::Mat histImg(histSize[0], histSize[0], CV_8U, cv::Scalar(225));
	//设置最高点为nbins的90%
	int hpt = static_cast<int>(0.9*histSize[0]);
	//给每个条目绘制一条垂直的线
	for (int h = 0; h < histSize[0]; h++){
		float binVal = hist.at<float>(h);
		int intensity = static_cast<int>(binVal*hpt / maxVal);
		//cout << intensity <<" "<<maxVal<< endl;
		//两点之间绘制一条直线
		cv::line(histImg, cv::Point(h, histSize[0]),
			cv::Point(h, histSize[0] - intensity),
			cv::Scalar::all(0));
	}
	return histImg;
}

cv::Mat Histogram1D::applyLookUp(const cv::Mat& image,	//输入图像
	const cv::Mat& lookup	//输出图像
	){
	//输出图片
	cv::Mat result;

	//应用查找表
	cv::LUT(image, lookup, result);
	return result;
}

//图片拉伸
cv::Mat Histogram1D::stretch(const cv::Mat &image, int minValue = 0){//minValue代表在多少一下算为0

	cv::MatND hist = getHistogram(image);

	//寻找左端
	int imin = 0;//左边下标
	for (; imin < histSize[0]; imin++){
		cout << hist.at<float>(imin) << endl;
		if (hist.at<float>(imin)>minValue) break;
	}

	//寻找右端
	int imax = histSize[0] - 1;//右边下标
	for (; imax> 0; imax--){
		cout << hist.at<float>(imax) << endl;
		if (hist.at<float>(imax)>minValue) break;
	}

	//创建查找表
	int dim(256);
	cv::Mat lookup(1, &dim, CV_8U);

	//填充查找表
	for (int i = 0; i < 256; i++){
		if (i < imin) lookup.at<uchar>(i) = 0;
		else if (i>imax) lookup.at<uchar>(i) = 255;
		else lookup.at<uchar>(i) = static_cast<uchar>(255.0*(i - imax) / (imax - imin) + 0.5);
	}

	//应用查找表
	cv::Mat result;
	result = applyLookUp(image, lookup);
	return result;
}

cv::Mat Histogram1D::applyLookUp(cv::Mat &image, const cv::Mat& lookup){
	cv::Mat result;

	cv::LUT(image, lookup, result);

	return result;
}