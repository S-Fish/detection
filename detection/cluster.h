#ifndef CLUSTER_H_
#define CLUSTER_H_

#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<iostream>
using namespace cv;
using namespace std;
namespace cluster{
	bool pretreat(const cv::Mat&input,cv::Mat& result);
	

}


#endif