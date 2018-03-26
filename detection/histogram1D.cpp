#include"histogram1D.h"



Histogram1D::Histogram1D(){
	this->histSize[0] = 256;
	this->hranges[0] = 0.0;
	this->hranges[1] = 255.0;
	ranges[0] = this->hranges;
	channels[0] = 0;//����0ͨ��
}
//����1Dֱ��ͼ
cv::MatND Histogram1D::getHistogram(const cv::Mat &image){
	cv::MatND hist;

	//����ֱ��ͼ������cv::�⺯������
	cv::calcHist(&image,
		1,				//���㵥��ͼ���ֱ��ͼ
		channels,		//ͨ������
		cv::Mat(),		//��ʹ��ͼ����Ϊ����
		hist,			//���ص���ֱ��ͼ
		1,				//����1D��ֱ��ͼ�������أ�
		histSize,		//�������	
		ranges			//����ֵ�ķ�Χ
		);
	return hist;
}

cv::Mat Histogram1D::getHistogramImage(const cv::Mat &image){

	//����ֱ��ͼ
	cv::MatND hist = getHistogram(image);

	//���������Сֵ
	double maxVal = 0;
	double minVal = 0;
	cv::minMaxLoc(hist, &minVal, &maxVal, 0, 0);

	//��ʾͼ
	cv::Mat histImg(histSize[0], histSize[0], CV_8U, cv::Scalar(225));
	//������ߵ�Ϊnbins��90%
	int hpt = static_cast<int>(0.9*histSize[0]);
	//��ÿ����Ŀ����һ����ֱ����
	for (int h = 0; h < histSize[0]; h++){
		float binVal = hist.at<float>(h);
		int intensity = static_cast<int>(binVal*hpt / maxVal);
		//cout << intensity <<" "<<maxVal<< endl;
		//����֮�����һ��ֱ��
		cv::line(histImg, cv::Point(h, histSize[0]),
			cv::Point(h, histSize[0] - intensity),
			cv::Scalar::all(0));
	}
	return histImg;
}

cv::Mat Histogram1D::applyLookUp(const cv::Mat& image,	//����ͼ��
	const cv::Mat& lookup	//���ͼ��
	){
	//���ͼƬ
	cv::Mat result;

	//Ӧ�ò��ұ�
	cv::LUT(image, lookup, result);
	return result;
}

//ͼƬ����
cv::Mat Histogram1D::stretch(const cv::Mat &image, int minValue = 0){//minValue�����ڶ���һ����Ϊ0

	cv::MatND hist = getHistogram(image);

	//Ѱ�����
	int imin = 0;//����±�
	for (; imin < histSize[0]; imin++){
		cout << hist.at<float>(imin) << endl;
		if (hist.at<float>(imin)>minValue) break;
	}

	//Ѱ���Ҷ�
	int imax = histSize[0] - 1;//�ұ��±�
	for (; imax> 0; imax--){
		cout << hist.at<float>(imax) << endl;
		if (hist.at<float>(imax)>minValue) break;
	}

	//�������ұ�
	int dim(256);
	cv::Mat lookup(1, &dim, CV_8U);

	//�����ұ�
	for (int i = 0; i < 256; i++){
		if (i < imin) lookup.at<uchar>(i) = 0;
		else if (i>imax) lookup.at<uchar>(i) = 255;
		else lookup.at<uchar>(i) = static_cast<uchar>(255.0*(i - imax) / (imax - imin) + 0.5);
	}

	//Ӧ�ò��ұ�
	cv::Mat result;
	result = applyLookUp(image, lookup);
	return result;
}

cv::Mat Histogram1D::applyLookUp(cv::Mat &image, const cv::Mat& lookup){
	cv::Mat result;

	cv::LUT(image, lookup, result);

	return result;
}