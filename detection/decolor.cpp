#include"decolor.h"
using namespace std;


//��÷�ˮ���㷨��Ҫ������
cv::Mat check::getWaterMask(const cv::Mat& binaryImage){

	//��ˮ�����ȱ��

	cv::Mat fg;//ǰ��ɫ
	cv::erode(binaryImage, fg, cv::Mat(), cv::Point(-1, -1), 1);
	//cv::imshow("fg", fg);


	cv::Mat bg;//����ɫ
	cv::dilate(binaryImage, bg, cv::Mat(), cv::Point(-1, -1), 2);
	cv::threshold(bg, bg, 1, 128, cv::THRESH_BINARY_INV);
	//cv::imshow("bg", bg);

	cv::Mat markers(binaryImage.size(), CV_8U, cv::Scalar(0));
	markers = fg + bg;//��ˮ��Ĳ���
	//cv::imshow("markers", markers);

	return markers;
}

//��ö�ֵ��ͼƬ
cv::Mat check::getBinaryImage(const cv::Mat& grayImage, int background){


	cv::Mat binaryImage;

	//���ľ�ĵ���ɫ�������˴����ֵ�ָ�
	cv::threshold(grayImage, binaryImage, 0, 255, THRESH_OTSU | THRESH_BINARY_INV);

	//��ȥ����ɫ
	cv::Mat binary2;
	cv::threshold(grayImage, binary2, background, 255, THRESH_BINARY);
	//cv::imshow("binary2",binary2);

	binaryImage = binaryImage&binary2;

	return binaryImage;

}

//��ˮ���㷨����
cv::Mat check::waterExcuteImage(const cv::Mat& mask, const cv::Mat& image){

	cv::Mat waterImage;
	check::WatershedSegmenter segenter;

	segenter.setMarkers(mask);

	waterImage = segenter.process(image);
	waterImage.convertTo(waterImage, CV_8U);

	//���ͽ�һЩϸС�����������
	cv::dilate(waterImage, waterImage, cv::Mat(), cv::Point(-1, -1), 5);

	return waterImage;
}


void check::clusterBinaryImage(cv::Mat &inImage, cv::Mat& outImage, int clusterCount){

	float step = 255 / (clusterCount - 1);
	if (!inImage.data) return;

	Mat samples(inImage.cols*inImage.rows, 1, CV_32FC3);
	Mat labels(inImage.cols*inImage.rows, 1, CV_32SC1);
	uchar* p;
	int k = 0;
	for (int m = 0; m < inImage.rows; m++)
	{
		p = inImage.ptr<uchar>(m);
		for (int n = 0; n < inImage.cols; n++)
		{
			samples.at<Vec3f>(k, 0)[0] = float(p[n * 3]);
			samples.at<Vec3f>(k, 0)[1] = float(p[n * 3 + 1]);
			samples.at<Vec3f>(k, 0)[2] = float(p[n * 3 + 2]);
			k++;
		}
	}

	Mat centers(clusterCount, 1, samples.type());
	kmeans(samples, clusterCount, labels, TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 10, 1.0), 3, KMEANS_PP_CENTERS, centers);
	//������֪��3�����࣬�ò�ͬ�ĻҶȲ��ʾ��   
	Mat img1(inImage.rows, inImage.cols, CV_8UC1);
	k = 0;
	for (int m = 0; m < img1.rows; m++)
	{
		p = img1.ptr<uchar>(m);
		for (int n = 0; n < img1.cols; n++)
		{
			int tt = labels.at<int>(k, 0);
			k++;
			p[n] = 255 - tt*step;
		}
	}

	threshold(img1, outImage, 0, 255, THRESH_BINARY | THRESH_OTSU);


}

//��ͼƬ����õ����Ա߽�ĻҶ�ͼƬ
cv::Mat check::getClearEdge(const cv::Mat& image){

	cv::Mat edgeImage;
	//������Canny����
	cv::Canny(image, edgeImage, 120, 130);
	return edgeImage;

}


//��ø��������Ŀ�����Ӿ���,����contoursΪvector<vector<cv::Point>>��˵
vector<Rect> check::getBuddingRect(cv::Mat& image, vector<vector<cv::Point>>& contours){

	cv::Mat copyImg = image.clone();

	//2.������ȷ������Ӿ���  
	int width = 0;
	int height = 0;
	int x = 0;
	int y = 0;

	//2.1 ����Rect���͵�vector����boundRect�������Ӿ��Σ���ʼ����СΪcontours.size()����������  
	vector<Rect> boundRect;


	//2.2 ����ÿ������  
	for (int i = 0; i < contours.size(); i++)
	{
		//2.3���������㼯��ȷ��������Ӿ��� 
	
		Rect rect = cv::boundingRect(Mat(contours[i]));

		//�����Ӿ��γ��Ϳ��Ǵ���80px������Ч��
		if (rect.width>10 && rect.height>10){
			boundRect.push_back(rect);
		}
	}

	return boundRect;
}
vector<Rect> check::getBuddingRect(cv::Mat& image, vector<Mat>& contours){

	cv::Mat copyImg = image.clone();

	//2.������ȷ������Ӿ���  
	int width = 0;
	int height = 0;
	int x = 0;
	int y = 0;

	//2.1 ����Rect���͵�vector����boundRect�������Ӿ��Σ���ʼ����СΪcontours.size()����������  
	vector<Rect> boundRect;


	//2.2 ����ÿ������  
	for (int i = 0; i < contours.size(); i++)
	{
		//2.3���������㼯��ȷ��������Ӿ��� 

		Rect rect = cv::boundingRect(Mat(contours[i]));

		//�����Ӿ��γ��Ϳ��Ǵ���80px������Ч��
		if (rect.width>10 && rect.height>10){
			boundRect.push_back(rect);
		}
	}

	return boundRect;
}

//�����ɫȱ��,��ֵ����ͼ����Ҫһ������ɫ
void check::detctColor(const char* path, int background){

	//��ȡͼƬ
	cv::Mat image = cv::imread(path);
	if (image.empty()){
		cout << "image is empty\n";
		return;
	}

	//cv::imshow("image", image);

	//ת��һ�³�Ϊ�Ҷ�ͼ��
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, CV_BGR2GRAY);
	//cv::imshow("GRAYImage", grayImage);


	//��ֵ��ͼƬ
	cv::Mat binaryImage = getBinaryImage(grayImage, background);
	//cv::imshow("binary", binaryImage);

	//�������
	cv::Mat markers = getWaterMask(binaryImage);


	//��ͼƬ��ʼ��ˮ����
	cv::Mat waterImage = waterExcuteImage(markers, image);
	//cv::imshow("water", waterImage);
	

	//ͨ����ˮ��ͼƬ���������������ͼƬ
	cv::Mat edgeImage = getClearEdge(waterImage);
	//cv::imshow("edge", edgeImage);
	//ͨ��������ͼƬ������������õ�����ʾ
	//Edge edgeTools;

	//���쳣����vector�޷��Զ�����
	//cv::vector<cv::vector<cv::Point>> contours; //= edgeTools.getContours(edgeImage)
	vector<Mat> contours(1000);//��Ҫ��Ϊ��������������ʵĵص㣬ʹ�õ�returnʱ�����vector�����ҵ���ȷ���ڴ�λ�ôӶ�����ȷ������
	cv::findContours(edgeImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	/*
	CV_CHAIN_APPROX_NONE�洢���е������㣬���ڵ������������λ�ò����1����max��abs��x1-x2����abs��y2-y1����==1
	CV_CHAIN_APPROX_SIMPLEѹ��ˮƽ���򣬴�ֱ���򣬶Խ��߷����Ԫ�أ�ֻ�����÷�����յ����꣬����һ����������ֻ��4����������������Ϣ
	CV_CHAIN_APPROX_TC89_L1��CV_CHAIN_APPROX_TC89_KCOSʹ��teh-Chinl chain �����㷨

	CV_RETR_EXTERNAL��ʾֻ���������
	CV_RETR_TREE����һ���ȼ����ṹ������
	*/


	//���������Ӿ���ͬʱ����ЩϸС�ľ��ι���
	vector<Rect> rects = getBuddingRect(image, contours);
	cout << rects.size() << endl;
	//rects = filterBuddingRect(rects);

	cutting(image, rects, path);


	cv::Mat rectImage = image.clone();


	for (int i = 0; i < rects.size(); i++){
		rectangle(rectImage, rects[i], Scalar(0, 0, 255), 1, 1, 0);//�þ��λ����δ�  
	}

	//cv::imshow("rect", rectImage);
	save(rectImage, path, 0);
	cout << rects.size();
}

void check::detctColor(const char* path){

	//��ȡͼƬ
	cv::Mat image = cv::imread(path);
	if (image.empty()){
		cout << "image is empty\n";
		return;
	}
	//cv::imshow("image", image);

	//ת��һ�³�Ϊ�Ҷ�ͼ��
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, CV_BGR2GRAY);
	//cv::imshow("GRAYImage", grayImage);


	//��ֵ��ͼƬ
	cv::Mat binaryImage = getBinaryImage(image, 19);
	//clusterBinaryImage(image, binaryImage, 4);
	//cv::imshow("binary", binaryImage);

	//�������
	cv::Mat markers = getWaterMask(binaryImage);


	//��ͼƬ��ʼ��ˮ����
	cv::Mat waterImage = waterExcuteImage(markers, image);
	//cv::imshow("water", waterImage);


	//ͨ����ˮ��ͼƬ���������������ͼƬ
	cv::Mat edgeImage = getClearEdge(waterImage);
	
	//ͨ��������ͼƬ������������õ�����ʾ
	//Edge edgeTools;
	std::vector<std::vector<cv::Point>> contours; //= edgeTools.getContours(edgeImage)
	cv::findContours(edgeImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//cout << "---" << endl;





	//���������Ӿ���ͬʱ����ЩϸС�ľ��ι���
	vector<Rect> rects = getBuddingRect(image, contours);
	cout << rects.size() << endl;
	//rects = filterBuddingRect(rects);

	cutting(image, rects, path);


	cv::Mat rectImage = image.clone();


	for (int i = 0; i < rects.size(); i++){
		rectangle(rectImage, rects[i], Scalar(0, 0, 255), 1, 1, 0);//�þ��λ����δ�  
	}

	cv::imshow("rect", rectImage);
	save(rectImage, path, 0);
	cout << rects.size();
}


void check::detctColor(const char* hsource, const char* htarget, const char* tail, int colorThresh, int background){

	char path[200], newpath[200];
	strcpy(path, hsource);
	strcat(path, tail);
	cout << path << endl;
	strcpy(newpath, htarget);
	strcat(newpath, tail);

	cout << newpath << endl;
	//��ȡͼƬ
	cv::Mat image = cv::imread(path);
	if (image.empty()){ return; }

	//ת��һ�³�Ϊ�Ҷ�ͼ��
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, CV_BGR2GRAY);



	//��ֵ��ͼƬ
	cv::Mat binaryImage = getBinaryImage(grayImage, background);

	//�������
	cv::Mat markers = getWaterMask(binaryImage);


	//��ͼƬ��ʼ��ˮ����
	cv::Mat waterImage = waterExcuteImage(markers, image);

	//ͨ����ˮ��ͼƬ���������������ͼƬ
	cv::Mat edgeImage = getClearEdge(waterImage);

	//ͨ��������ͼƬ������������õ�����ʾ
	//Edge edgeTools;
	std::vector<std::vector<cv::Point>> contours;// = edgeTools.getContours(edgeImage);
	cv::findContours(edgeImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);


	//���������Ӿ���ͬʱ����ЩϸС�ľ��ι���
	vector<Rect> rects = getBuddingRect(image, contours);
	//cout << rects.size() << endl;

	cutting(image, rects, newpath);


	cv::Mat rectImage = image.clone();

	//cout << rects.size();
	for (int i = 0; i < rects.size(); i++){
		rectangle(rectImage, rects[i], Scalar(0, 0, 255), 1, 1, 0);//�þ��λ����δ�  
	}

	save(rectImage, newpath, 0);


}



void  check::cutting(const cv::Mat&image, Vector<Rect> Rects, const char* path){

	cv::Mat ROI_Image;

	for (int i = 0; i < Rects.size(); i++){
		image(Rects[i]).copyTo(ROI_Image);
		save(ROI_Image, path, i + 1);
	}

}

void check::save(const cv::Mat&image, const char* path, int i){

	IplImage *ipl_roi_img = &IplImage(image);
	//save processed img
	char tmp[100] = "\0";
	string newpath(path);
	newpath.replace(newpath.find("."), 4, "");
	newpath.append("_%d.jpg");
	char* ospath = (char*)newpath.c_str();
	sprintf(tmp, ospath, i);
	cvSaveImage(tmp, ipl_roi_img);

}


void check::WatershedSegmenter::setMarkers(const cv::Mat& markerImage){
	markerImage.convertTo(markers, CV_32S);
}

cv::Mat check::WatershedSegmenter::process(const cv::Mat&image){
	cv::watershed(image, markers);
	return markers;
}
