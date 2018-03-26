#include"decolor.h"
using namespace std;


//获得分水岭算法需要的掩码
cv::Mat check::getWaterMask(const cv::Mat& binaryImage){

	//分水岭提出缺陷

	cv::Mat fg;//前景色
	cv::erode(binaryImage, fg, cv::Mat(), cv::Point(-1, -1), 1);
	//cv::imshow("fg", fg);


	cv::Mat bg;//背景色
	cv::dilate(binaryImage, bg, cv::Mat(), cv::Point(-1, -1), 2);
	cv::threshold(bg, bg, 1, 128, cv::THRESH_BINARY_INV);
	//cv::imshow("bg", bg);

	cv::Mat markers(binaryImage.size(), CV_8U, cv::Scalar(0));
	markers = fg + bg;//分水岭的参数
	//cv::imshow("markers", markers);

	return markers;
}

//获得二值化图片
cv::Mat check::getBinaryImage(const cv::Mat& grayImage, int background){


	cv::Mat binaryImage;

	//获得木材的颜色，采用了大津阈值分割
	cv::threshold(grayImage, binaryImage, 0, 255, THRESH_OTSU | THRESH_BINARY_INV);

	//除去背景色
	cv::Mat binary2;
	cv::threshold(grayImage, binary2, background, 255, THRESH_BINARY);
	//cv::imshow("binary2",binary2);

	binaryImage = binaryImage&binary2;

	return binaryImage;

}

//分水岭算法计算
cv::Mat check::waterExcuteImage(const cv::Mat& mask, const cv::Mat& image){

	cv::Mat waterImage;
	check::WatershedSegmenter segenter;

	segenter.setMarkers(mask);

	waterImage = segenter.process(image);
	waterImage.convertTo(waterImage, CV_8U);

	//膨胀将一些细小的区域合起来
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
	//我们已知有3个聚类，用不同的灰度层表示。   
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

//将图片处理得到明显边界的灰度图片
cv::Mat check::getClearEdge(const cv::Mat& image){

	cv::Mat edgeImage;
	//采用了Canny算子
	cv::Canny(image, edgeImage, 120, 130);
	return edgeImage;

}


//获得各个轮廓的可能外接矩形,对于contours为vector<vector<cv::Point>>来说
vector<Rect> check::getBuddingRect(cv::Mat& image, vector<vector<cv::Point>>& contours){

	cv::Mat copyImg = image.clone();

	//2.由轮廓确定正外接矩形  
	int width = 0;
	int height = 0;
	int x = 0;
	int y = 0;

	//2.1 定义Rect类型的vector容器boundRect存放正外接矩形，初始化大小为contours.size()即轮廓个数  
	vector<Rect> boundRect;


	//2.2 遍历每个轮廓  
	for (int i = 0; i < contours.size(); i++)
	{
		//2.3由轮廓（点集）确定出正外接矩形 
	
		Rect rect = cv::boundingRect(Mat(contours[i]));

		//如果外接矩形长和宽都是大于80px就是有效的
		if (rect.width>10 && rect.height>10){
			boundRect.push_back(rect);
		}
	}

	return boundRect;
}
vector<Rect> check::getBuddingRect(cv::Mat& image, vector<Mat>& contours){

	cv::Mat copyImg = image.clone();

	//2.由轮廓确定正外接矩形  
	int width = 0;
	int height = 0;
	int x = 0;
	int y = 0;

	//2.1 定义Rect类型的vector容器boundRect存放正外接矩形，初始化大小为contours.size()即轮廓个数  
	vector<Rect> boundRect;


	//2.2 遍历每个轮廓  
	for (int i = 0; i < contours.size(); i++)
	{
		//2.3由轮廓（点集）确定出正外接矩形 

		Rect rect = cv::boundingRect(Mat(contours[i]));

		//如果外接矩形长和宽都是大于80px就是有效的
		if (rect.width>10 && rect.height>10){
			boundRect.push_back(rect);
		}
	}

	return boundRect;
}

//检测颜色缺陷,二值化的图像需要一个背景色
void check::detctColor(const char* path, int background){

	//读取图片
	cv::Mat image = cv::imread(path);
	if (image.empty()){
		cout << "image is empty\n";
		return;
	}

	//cv::imshow("image", image);

	//转化一下成为灰度图像
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, CV_BGR2GRAY);
	//cv::imshow("GRAYImage", grayImage);


	//二值化图片
	cv::Mat binaryImage = getBinaryImage(grayImage, background);
	//cv::imshow("binary", binaryImage);

	//获得掩码
	cv::Mat markers = getWaterMask(binaryImage);


	//将图片开始分水计算
	cv::Mat waterImage = waterExcuteImage(markers, image);
	//cv::imshow("water", waterImage);
	

	//通过分水的图片来获得清晰的轮廓图片
	cv::Mat edgeImage = getClearEdge(waterImage);
	//cv::imshow("edge", edgeImage);
	//通过轮廓的图片来获得轮廓，用点来表示
	//Edge edgeTools;

	//有异常存在vector无法自动析构
	//cv::vector<cv::vector<cv::Point>> contours; //= edgeTools.getContours(edgeImage)
	vector<Mat> contours(1000);//主要是为了在这边析构合适的地点，使得当return时候这个vector可以找到正确的内存位置从而被正确的析构
	cv::findContours(edgeImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	/*
	CV_CHAIN_APPROX_NONE存储所有的轮廓点，相邻的两个点的像素位置差不超过1，即max（abs（x1-x2），abs（y2-y1））==1
	CV_CHAIN_APPROX_SIMPLE压缩水平方向，垂直方向，对角线方向的元素，只保留该方向的终点坐标，例如一个矩形轮廓只需4个点来保存轮廓信息
	CV_CHAIN_APPROX_TC89_L1，CV_CHAIN_APPROX_TC89_KCOS使用teh-Chinl chain 近似算法

	CV_RETR_EXTERNAL表示只检测外轮廓
	CV_RETR_TREE建立一个等级树结构的轮廓
	*/


	//获得轮廓外接矩阵，同时将那些细小的矩形过滤
	vector<Rect> rects = getBuddingRect(image, contours);
	cout << rects.size() << endl;
	//rects = filterBuddingRect(rects);

	cutting(image, rects, path);


	cv::Mat rectImage = image.clone();


	for (int i = 0; i < rects.size(); i++){
		rectangle(rectImage, rects[i], Scalar(0, 0, 255), 1, 1, 0);//用矩形画矩形窗  
	}

	//cv::imshow("rect", rectImage);
	save(rectImage, path, 0);
	cout << rects.size();
}

void check::detctColor(const char* path){

	//读取图片
	cv::Mat image = cv::imread(path);
	if (image.empty()){
		cout << "image is empty\n";
		return;
	}
	//cv::imshow("image", image);

	//转化一下成为灰度图像
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, CV_BGR2GRAY);
	//cv::imshow("GRAYImage", grayImage);


	//二值化图片
	cv::Mat binaryImage = getBinaryImage(image, 19);
	//clusterBinaryImage(image, binaryImage, 4);
	//cv::imshow("binary", binaryImage);

	//获得掩码
	cv::Mat markers = getWaterMask(binaryImage);


	//将图片开始分水计算
	cv::Mat waterImage = waterExcuteImage(markers, image);
	//cv::imshow("water", waterImage);


	//通过分水的图片来获得清晰的轮廓图片
	cv::Mat edgeImage = getClearEdge(waterImage);
	
	//通过轮廓的图片来获得轮廓，用点来表示
	//Edge edgeTools;
	std::vector<std::vector<cv::Point>> contours; //= edgeTools.getContours(edgeImage)
	cv::findContours(edgeImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//cout << "---" << endl;





	//获得轮廓外接矩阵，同时将那些细小的矩形过滤
	vector<Rect> rects = getBuddingRect(image, contours);
	cout << rects.size() << endl;
	//rects = filterBuddingRect(rects);

	cutting(image, rects, path);


	cv::Mat rectImage = image.clone();


	for (int i = 0; i < rects.size(); i++){
		rectangle(rectImage, rects[i], Scalar(0, 0, 255), 1, 1, 0);//用矩形画矩形窗  
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
	//读取图片
	cv::Mat image = cv::imread(path);
	if (image.empty()){ return; }

	//转化一下成为灰度图像
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, CV_BGR2GRAY);



	//二值化图片
	cv::Mat binaryImage = getBinaryImage(grayImage, background);

	//获得掩码
	cv::Mat markers = getWaterMask(binaryImage);


	//将图片开始分水计算
	cv::Mat waterImage = waterExcuteImage(markers, image);

	//通过分水的图片来获得清晰的轮廓图片
	cv::Mat edgeImage = getClearEdge(waterImage);

	//通过轮廓的图片来获得轮廓，用点来表示
	//Edge edgeTools;
	std::vector<std::vector<cv::Point>> contours;// = edgeTools.getContours(edgeImage);
	cv::findContours(edgeImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);


	//获得轮廓外接矩阵，同时将那些细小的矩形过滤
	vector<Rect> rects = getBuddingRect(image, contours);
	//cout << rects.size() << endl;

	cutting(image, rects, newpath);


	cv::Mat rectImage = image.clone();

	//cout << rects.size();
	for (int i = 0; i < rects.size(); i++){
		rectangle(rectImage, rects[i], Scalar(0, 0, 255), 1, 1, 0);//用矩形画矩形窗  
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
