#include"cluster.h"
#include"histogram1D.h"


bool cluster::pretreat(const cv::Mat&input, cv::Mat& result){

	Mat image = input.clone();
	if (input.channels() != 1){
		cv::cvtColor(image, image, CV_BGR2GRAY);
	}
	Histogram1D his;
	cv::MatND hisND = his.getHistogram(image);
	int maxIdx=0;
	float max = hisND.at<float>(0);

	for (int i = 1; i < 256; i++){
		if (hisND.at<float>(i)>max){
			maxIdx = i;
			max = hisND.at<float>(i);
		}
	}

	result = image.clone();
	int n1 = image.rows;
	int nc = image.cols;
	

	for (int i = 0; i < n1; i++){
		uchar* data = image.ptr<uchar>(i);
		uchar* resultData = result.ptr<uchar>(i);
		for (int j = 0; j < nc; j++){
			resultData[j] =abs( data[j] - maxIdx);
			//cout << int(data[j]) << "-" << maxIdx << "=" << int(resultData[j]) << endl;
		}
		
	}


	return true;
}