#ifndef DECOLOR_H_
#define DECOLOR_H_

#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<iostream>
using namespace cv;
using namespace std;
extern string message;

namespace check{
	struct ReactPoint
	{
		Point leftTop;

		Point rightDown;
	};
	class WatershedSegmenter{
	private:
		cv::Mat markers;
	public:
		void setMarkers(const cv::Mat& markerImage);

		cv::Mat process(const cv::Mat&image);

	};

	cv::Mat getWaterMask(const cv::Mat& binaryImage);

	cv::Mat getBinaryImage(const cv::Mat& grayImage, int background = 40);

	cv::Mat waterExcuteImage(const cv::Mat& mask, const cv::Mat& image);

	cv::Mat getClearEdge(const cv::Mat& image);
	void clusterBinaryImage(cv::Mat &inImage, cv::Mat& outImage, int clusterCount);

	vector<Rect> getBuddingRect(cv::Mat& image, vector<vector<cv::Point>>& contours);
	vector<Rect> getBuddingRect(cv::Mat& image, vector<Mat>& contours);
	vector<Rect> detctColor(const char* path, int background = 40);

	//用8*8的矩形腐蚀
	void detctColorErode(const char* path, int background = 40);

	void detctColor(const char* path);

	//hsource+tail是处理的图片目录，htarget+tail是得到的缺陷目录
	void detctColor(const char* hsource, const char* htarget, const char* tail, int colorThresh = 90, int background = 40);

	void cutting(const cv::Mat&image, Vector<Rect> Rects, const char* path);


	void save(const cv::Mat&image, const char* path, int i);//i是后缀
}


#endif