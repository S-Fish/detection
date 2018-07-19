#include"decolor.h"
#include"RectMerge.h"
#include<time.h>
#include<fstream>
using namespace std;



//采用5领域方法去除噪点上
//白色点在kennel*kennel矩形中占有的比例不足number的时候转化为黑色,0<number<1;
void clearLineByRow(const Mat& image,Mat& result,int number){
	Mat tmp = image.clone();
	int rows = image.rows;
	int cols = image.cols ;
	number = cols*0.8;
	int count,begin;
	for (int i = 0; i < rows; i++){
		
		uchar* line = tmp.ptr<uchar>(i);
		
		for (int j = 0; j < cols; j++){
			count = 0;
			begin = j;
			while (j<cols&&line[j]==255)
			{
				count++;
				j++;
			}
			if (count > number){
				for (int l = begin; l<cols&&l < j; l++){
					line[l] = 0;
				}
			}
			
		}
	}
	result = tmp.clone();

	
}

void clearLine(const Mat& image, Mat& result, int number){
	Mat tmp = image.clone();
	int rows = image.rows;
	int cols = image.cols;
	int count, begin;
	for (int i = 0; i < rows; i++){

		uchar* line = tmp.ptr<uchar>(i);

		for (int j = 0; j < cols; j++){
			count = 0;
			begin = j;
			while (line[j] == 255)
			{
				count++;
				j++;
			}
			if (count < number){
				for (int l = begin; l < j; l++){
					line[l] = 0;
				}
			}

		}
	}
	result = tmp.clone();


}

//竖线来判断
void clearLineByCol(const Mat& image, Mat& result, int number){
	Mat tmp = image.clone();
	int rows = image.rows;
	int cols = image.cols;
	int count, begin;
	for (int i = 0; i < cols; i++){
		for (int j = 0; j < rows; j++){
			count = 0;
			while (j<rows&&tmp.at<uchar>(j, i) == 255){
				count++;
				j++;
			}
			
			if (count > number){
				for (int l = j - count; l < j&&l<rows; l++){
					tmp.at<uchar>(l, i) = 0;
				}
			}

		}
	
	}
	result = tmp.clone();


}
//down左边下线,up是右边下线
void clearLineByAear(const Mat& image, Mat& result,int length,float down,float up){
	Mat tmp = image.clone();
	int rows = image.rows;
	int cols = image.cols;
	int downI, upI, tmpInt;
	int count, begin,wCount,end;
	for (int i = 0; i < rows; i=i+length){

		uchar* line = tmp.ptr<uchar>(i);

		for (int j = 0; j < cols; j++){
			count = 0;
			begin = j;
			while (line[j] == 255)
			{
				count++;
				j++;
			}
			if (count != 0){
				downI = down*count;
				upI = up*count;
				tmpInt = 1;
				
				//后面开始的位置，检测的长度是2*count,开始位置向后减0.5*count
				begin = begin - count / 2;
				end = begin + count / 2;
				if (begin <=0 ) begin = 0;
				if (end > cols) end = cols;
				for (; tmpInt < length; tmpInt++){
					uchar* lineT = tmp.ptr<uchar>(i+tmpInt);
					wCount = 0;
					for (int i = begin; i < end; i++){
						if (lineT[i] == 255){
							wCount++;
						}
					}
					//判断是不是落在范围内，不是的话就要跳出循环了，是的继续
					if (wCount<downI || wCount>upI) break;
				}
				//正常跳出，染色
				if (tmpInt == length){
					for (int i = 0; i < length; i++){
						for (int j = begin; j < end; j++){
							(line + i)[j] = 0;
						}
					}
				}
				
			}

		}
	}
	result = tmp.clone();


}


//获得分水岭算法需要的掩码
cv::Mat check::getWaterMask(const cv::Mat& binaryImage){

	//分水岭提出缺陷
	cv::Mat fg;//前景色
	
	//clearLine(binaryImage, fg, 10);
	//cv::erode(fg, fg, cv::Mat(), cv::Point(-1, -1), 1);
	//cv::morphologyEx(binaryImage, binaryImage, MORPH_OPEN, cv::Mat());
	cv::erode(binaryImage, fg, cv::Mat(), cv::Point(-1, -1), 3);
	//cv::imshow("fg", fg);
	

	cv::Mat bg;//背景色
	cv::dilate(binaryImage, bg, cv::Mat(), cv::Point(-1, -1), 5);
	cv::threshold(bg, bg, 1, 128, cv::THRESH_BINARY_INV);
	//cv::imshow("bg", bg);

	cv::Mat markers(binaryImage.size(), CV_8U, cv::Scalar(0));
	markers = fg + bg;//分水岭的参数
	//cv::imshow("markers", markers);

	return markers;
}


//adjust
void adjustS(int* t,int begin,int length){
	//正常的套路是需要回溯的，即当父节点查完后子节点也要查
	//但是针对我们的需求只要找到一个最大值就行，不用回溯
	//同时我们对这个也只是使用
}

//利用堆来寻找排序来寻找
//这个数组的特点是中间的值比较大
int getMaxLine(int* arry,int length){
	//原数组不要动,copy一个新数组,可以,int的直接赋值效率比memcpy高
	int * t = new int[length+2];//为了使得下标由1开始+1,为了不出现有一个节点没有左子树情况+1
	for (int i = 0; i < length; i++){
		t[i+1] = arry[i];
	}
	t[0] = 0;
	t[length + 1] = 0;

	for (int i = length / 2; i >= 1; i--){
		
		if (t[i] < t[i * 2]) t[i] = t[i * 2];
		if (t[i] < t[i * 2+1]) t[i] = t[i * 2+1];
	}
	
	int maxL = t[1];

	delete[] t;
	return maxL;

}

//获得二值化图片
cv::Mat check::getBinaryImage(const cv::Mat& grayImage, int background){


	cv::Mat binaryImage;

	//获得木材的颜色，采用了大津阈值分割
	cv::threshold(grayImage, binaryImage, 0, 255, THRESH_OTSU | THRESH_BINARY_INV);

	//除去背景色
	//cv::Mat binary2;
	//cv::threshold(grayImage, binary2, background, 255, THRESH_BINARY);
	//cv::imshow("binary2",binary2);

	//binaryImage = binaryImage&binary2;
	
	int * rowValues, *colValues;
	rowValues = new int[binaryImage.rows]();
	colValues = new int[binaryImage.cols]();
	int x1=0, y1=0;
	int x2=0, y2=0;
	bool flag=false;

	for (int i = 0; i < binaryImage.rows; i++){
		uchar* line = binaryImage.ptr<uchar>(i);
		for (int j = 0; j < binaryImage.cols; j++){
			if (line[j] == 0){
				rowValues[i]++;
				colValues[j]++;
			}
		}
	}


	int rowL= getMaxLine(rowValues, binaryImage.rows)*0.8;

	int colL= getMaxLine(colValues, binaryImage.cols)*0.8;
	

	
	for (int i = 0; i < binaryImage.rows; i++){
		if (rowValues[i] >= rowL){
			y1 = i;
			break;
		}
	}

	for (int i = binaryImage.rows-1; i >=0; i--){
		if (rowValues[i] >= rowL){
			y2 = i;
			break;
		}
	}

	for (int i = 0; i < binaryImage.cols; i++){
		if (colValues[i] >= colL){
			x1 = i;
			break;
		}
	}

	for (int i = binaryImage.cols-1; i >=0; i--){
		if (colValues[i] >= colL){
			x2 = i;
			break;
		}
	}
	delete[] rowValues;
	delete[] colValues;
	/*
	for (int i = 0; i < binaryImage.rows; i++){
		uchar* line = binaryImage.ptr<uchar>(i);
		for (int j = 0; j < binaryImage.cols; j++){
			if (line[j] == 0){
				flag = true;
				break;
			}
		}
		if (flag){ 
			y1 = i;
			flag = false;
			break;
		}
	}

	for (int i = binaryImage.rows-1; i >= 0; i--){
		uchar* line = binaryImage.ptr<uchar>(i);
		for (int j = 0; j < binaryImage.cols; j++){
			if (line[j] == 0){
				flag = true;
				break;
			}
		}
		if (flag){
			y2 = i;
			flag = false;
			break;
		}
	}

	for (int i = 0; i < binaryImage.cols; i++){
	
		for (int j = 0; j < binaryImage.rows; j++){
			if (binaryImage.at<uchar>(j, i) == 0){
				flag = true;
				break;
			}
		}
		if (flag){
			x1 = i;
			flag = false;
			break;
		}
	}

	for (int i = binaryImage.cols-1; i >=0; i--){

		for (int j = 0; j < binaryImage.rows; j++){
			if (binaryImage.at<uchar>(j, i) == 0){
				flag = true;
				break;
			}
		}
		if (flag){
			x2 = i;
			break;
		}
	}
	*/
	cv::Mat tmp(binaryImage.size(),CV_8UC1,Scalar(0));
	cv::Rect ROI(x1, y1, x2 - x1, y2 - y1);
	rectangle(tmp, ROI, Scalar(255, 255, 255), CV_FILLED);

	binaryImage = binaryImage&tmp;
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
		if (rect.width>80 && rect.height>80){
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
		float flag = rect.width*1.0 / rect.height;
		if (rect.width>10 && rect.height>10 && (flag >= 0.3&&flag <= 3)){//
			double area = contourArea(contours[i], false);

			boundRect.push_back(rect);
		}
		
	}

	return boundRect;
}

/*//剔除直线
void rmoveLine(const cv::Mat& inputImage,cv::Mat& outputImage){
	cv::Mat temp = inputImage.clone();

	
	vector<cv::Vec2f> lines;
	cv::HoughLines(inputImage, lines, 1, CV_PI/180, 180);
	vector<cv::Vec2f>::const_iterator it = lines.begin();

	while (it != lines.end())
	{
		float rho = (*it)[0]; // first element is distance rho
		float theta = (*it)[1]; // second element is angle theta
		if (theta < CV_PI / 4. || theta > 3.*CV_PI / 4.)// ~vertical line
		{
			// point of intersection of the line with first row
			cv::Point pt1(rho / cos(theta), 0);
			// point of intersection of the line with last row
			cv::Point pt2((rho - temp.rows * sin(theta)) / cos(theta), temp.rows);
			// draw a black line
			cv::line(temp, pt1, pt2, cv::Scalar(0), 1);
		}
		else
		{ // ~horizontal line
			// point of intersection of the
			// line with first column
			cv::Point pt1(0, rho / sin(theta));
			// point of intersection of the line with last column
			cv::Point pt2(temp.cols, (rho - temp.cols * cos(theta)) / sin(theta));
			// draw a black line
			cv::line(temp, pt1, pt2, cv::Scalar(0), 1);
		}
		++it;
	}
	outputImage = temp.clone();
}
*/



Mat fillHole(const Mat srcBw)
{
	Size m_Size = srcBw.size();
	Mat Temp = Mat::zeros(m_Size.height + 2, m_Size.width + 2, srcBw.type());//延展图像  
	srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));

	cv::floodFill(Temp, Point(0, 0), Scalar(255));

	Mat cutImg;//裁剪延展的图像  
	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);

	Mat dstBw = srcBw | (~cutImg);
	return dstBw;
}

//切割图片，为了分割背景
void clearBackground(const cv::Mat& inputImage,cv::Mat& outputImage){


	if (!inputImage.data)
	{
		std::cout << "No data!" << std::endl;
		return ;
	}

	cv::Mat temp = inputImage.clone();

	Mat ROI = inputImage;

	cvtColor(temp, temp, CV_BGR2GRAY);
	Mat gaussImage;
	//medianBlur(midimage, gaussImage, 15);
	cv::Mat binary;
	threshold(temp, binary, 60, 255, THRESH_BINARY);
	Mat result = fillHole(binary);

	//去除噪点
	dilate(result, result, cv::Mat(), cv::Point(-1, -1), 4);

	//Canny(result, result, 100, 300, 3);
	vector <Mat> contours(500);

	findContours(result, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//最外层轮廓  

	Mat dst[500];
	for (int i = 0; i < contours.size(); i++)
	{
		Rect r = boundingRect(Mat(contours[i]));
		dst[i] = ROI(r);
	}

	for (int i = 0; i < contours.size(); i++)
		for (int j = 0; j < contours.size() - 1 - i; j++)
		{
			if ((dst[j].cols * dst[j].rows)< (dst[j + 1].cols * dst[j + 1].rows))
			{
				swap(dst[j], dst[j + 1]);
			}
			else continue;
		}
	outputImage = dst[0];
}



//检测颜色缺陷,二值化的图像需要一个背景色

vector<Rect> check::detctColor(const char* path, int background){
	
	string reactFilePath ="E:\\项目\\木材检测\\新建文件夹\\测试文件\\分析中间";
	string fileName=path;
	size_t t = fileName.find_last_of("\\");
	fileName = fileName.substr(t);
	reactFilePath = reactFilePath + fileName;
	clock_t a, b;
	a = clock();
	int bNumber=0;
	vector<Rect> newRects;
	//读取图片
	cv::Mat image = cv::imread(path);
	
	if (image.empty()){
		cout << "image is empty\n";
		return newRects;
	}
	b = clock();
	cout << "clock:" << b - a <<"/"<< CLOCKS_PER_SEC<<"s\n";

	//除去背景
	//clearBackground(image, image);

	cv::Mat realImage = image;
	while (image.cols>10000 || image.rows>10000){
		resize(image, image, Size(image.cols / 2, image.rows / 2), 0, 0, INTER_LINEAR);
		bNumber++;
	}



	//转化一下成为灰度图像	
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, CV_BGR2GRAY);

	

	//二值化图片
	cv::Mat binaryImage = getBinaryImage(grayImage, background);
	save(binaryImage, reactFilePath.c_str(), 1);
	cv::dilate(binaryImage, binaryImage, cv::Mat(), cv::Point(),3);

	
	//cv::morphologyEx(binaryImage, binaryImage, MORPH_OPEN, cv::Mat());
	save(binaryImage, reactFilePath.c_str(), 1);

	clearLineByRow(binaryImage, binaryImage, int(binaryImage.rows*0.4));
	clearLineByCol(binaryImage, binaryImage, 300);
	clearLine(binaryImage, binaryImage, 7);
	
	//end = clock();


	save(binaryImage, reactFilePath.c_str(), 2);
	
	//获得掩码
	cv::Mat markers = getWaterMask(binaryImage);


	//将图片开始分水计算
	cv::Mat waterImage = waterExcuteImage(markers, image);

	

	//通过分水的图片来获得清晰的轮廓图片
	cv::Mat edgeImage = getClearEdge(waterImage);
	save(edgeImage, reactFilePath.c_str(), 3);

	//通过轮廓的图片来获得轮廓，用点来表示
	//Edge edgeTools;

	//有异常存在vector无法自动析构
	//cv::vector<cv::vector<cv::Point>> contours; //= edgeTools.getContours(edgeImage)
	vector<Mat> contours(1000);//主要是为了在这边析构合适的地点，使得当return时候这个vector可以找到正确的内存位置从而被正确的析构
	cv::findContours(edgeImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//end = clock();

	//cout << "findContours:" << end - start << "/" << CLOCKS_PER_SEC << " (s) " << endl;
	
	
	//CV_CHAIN_APPROX_NONE存储所有的轮廓点，相邻的两个点的像素位置差不超过1，即max（abs（x1-x2），abs（y2-y1））==1
	//CV_CHAIN_APPROX_SIMPLE压缩水平方向，垂直方向，对角线方向的元素，只保留该方向的终点坐标，例如一个矩形轮廓只需4个点来保存轮廓信息
	//CV_CHAIN_APPROX_TC89_L1，CV_CHAIN_APPROX_TC89_KCOS使用teh-Chinl chain 近似算法

	//CV_RETR_EXTERNAL表示只检测外轮廓
	//CV_RETR_TREE建立一个等级树结构的轮廓
	

	
	//获得轮廓外接矩阵，同时将那些细小的矩形过滤
	vector<Rect> rects = getBuddingRect(image, contours);
	
	cout << rects.size();
     rects=rectMerge::mergeRect(rects);


	cv::Mat rectImage = image.clone();

	
	for (int i = 0; i < rects.size(); i++){
		rectangle(rectImage, rects[i], Scalar(0, 0, 255), 1, 1, 0);//用矩形画矩形窗  
	}
	save(rectImage, reactFilePath.c_str(), 0);
	
	
	cout <<" "<< rects.size()<<" ";
	cout << path << " cutted" << endl;
	message += " " + to_string(rects.size()) + " " + path + " cutted\n";

	
	for (int i = 0; i < rects.size(); i++){
		newRects.push_back(Rect(rects[i].x << bNumber, rects[i].y << bNumber, rects[i].width << bNumber, rects[i].height << bNumber));
	}
	//if (newRects.size()<=10) cutting(realImage, newRects, reactFilePath.c_str());
	b = clock();
	cout << "all:" << b - a <<"/"<< CLOCKS_PER_SEC << "s\n";
	return newRects;
}







/*下面的不用调用，被淘汰了*/
void check::detctColorErode(const char* path, int background){
	//读取图片
	cv::Mat image = cv::imread(path);
	if (image.empty()){
		cout << "image is empty\n";
		return;
	}



	while (image.cols>10000 || image.rows>10000){
		resize(image, image, Size(image.cols / 2, image.rows / 2), 0, 0, INTER_LINEAR);
	}
	cv::imshow("image", image);
	cv::waitKey(0);
	return;

	//转化一下成为灰度图像
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, CV_BGR2GRAY);
	//cv::imshow("GRAYImage", grayImage);


	//二值化图片
	cv::Mat binaryImage = getBinaryImage(grayImage, background);


	threshold(grayImage, binaryImage, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY_INV);
	
	cv::Mat edgeImage;
	Mat element = getStructuringElement(0, Size(8, 8));//MORPH_CROSS=1 MORPH_ELLIPSE=2 MORPH_RECT=0
	erode(binaryImage, edgeImage, element);
	save(edgeImage, path, 0);
	edgeImage = getClearEdge(edgeImage);
	

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
	//cout << rects.size() << endl;
	//rects = filterBuddingRect(rects);

	//cutting(image, rects, path);


	cv::Mat rectImage = image.clone();


	for (int i = 0; i < rects.size(); i++){
		rectangle(rectImage, rects[i], Scalar(0, 0, 255), 1, 1, 0);//用矩形画矩形窗  
	}

	//save(rectImage, path, 0);
	cout << path<<" cutover\n";
}
void check::detctColor(const char* path){

	//读取图片
	cv::Mat image = cv::imread(path);
	if (image.empty()){
		cout << "image is empty\n";
		return;
	}
	while (image.cols>10000 || image.rows>10000){
		resize(image, image, Size(image.cols / 2, image.rows / 2), 0, 0, INTER_LINEAR);
	}
	cv::imshow("image", image);
	cv::waitKey(0);
	return;
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

	//cutting(image, rects, newpath);


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
