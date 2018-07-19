#include"decolor.h"
#include"RectMerge.h"
#include<time.h>
#include<fstream>
using namespace std;



//����5���򷽷�ȥ�������
//��ɫ����kennel*kennel������ռ�еı�������number��ʱ��ת��Ϊ��ɫ,0<number<1;
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

//�������ж�
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
//down�������,up���ұ�����
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
				
				//���濪ʼ��λ�ã����ĳ�����2*count,��ʼλ������0.5*count
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
					//�ж��ǲ������ڷ�Χ�ڣ����ǵĻ���Ҫ����ѭ���ˣ��ǵļ���
					if (wCount<downI || wCount>upI) break;
				}
				//����������Ⱦɫ
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


//��÷�ˮ���㷨��Ҫ������
cv::Mat check::getWaterMask(const cv::Mat& binaryImage){

	//��ˮ�����ȱ��
	cv::Mat fg;//ǰ��ɫ
	
	//clearLine(binaryImage, fg, 10);
	//cv::erode(fg, fg, cv::Mat(), cv::Point(-1, -1), 1);
	//cv::morphologyEx(binaryImage, binaryImage, MORPH_OPEN, cv::Mat());
	cv::erode(binaryImage, fg, cv::Mat(), cv::Point(-1, -1), 3);
	//cv::imshow("fg", fg);
	

	cv::Mat bg;//����ɫ
	cv::dilate(binaryImage, bg, cv::Mat(), cv::Point(-1, -1), 5);
	cv::threshold(bg, bg, 1, 128, cv::THRESH_BINARY_INV);
	//cv::imshow("bg", bg);

	cv::Mat markers(binaryImage.size(), CV_8U, cv::Scalar(0));
	markers = fg + bg;//��ˮ��Ĳ���
	//cv::imshow("markers", markers);

	return markers;
}


//adjust
void adjustS(int* t,int begin,int length){
	//��������·����Ҫ���ݵģ��������ڵ������ӽڵ�ҲҪ��
	//����������ǵ�����ֻҪ�ҵ�һ�����ֵ���У����û���
	//ͬʱ���Ƕ����Ҳֻ��ʹ��
}

//���ö���Ѱ��������Ѱ��
//���������ص����м��ֵ�Ƚϴ�
int getMaxLine(int* arry,int length){
	//ԭ���鲻Ҫ��,copyһ��������,����,int��ֱ�Ӹ�ֵЧ�ʱ�memcpy��
	int * t = new int[length+2];//Ϊ��ʹ���±���1��ʼ+1,Ϊ�˲�������һ���ڵ�û�����������+1
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

//��ö�ֵ��ͼƬ
cv::Mat check::getBinaryImage(const cv::Mat& grayImage, int background){


	cv::Mat binaryImage;

	//���ľ�ĵ���ɫ�������˴����ֵ�ָ�
	cv::threshold(grayImage, binaryImage, 0, 255, THRESH_OTSU | THRESH_BINARY_INV);

	//��ȥ����ɫ
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
		if (rect.width>80 && rect.height>80){
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
		float flag = rect.width*1.0 / rect.height;
		if (rect.width>10 && rect.height>10 && (flag >= 0.3&&flag <= 3)){//
			double area = contourArea(contours[i], false);

			boundRect.push_back(rect);
		}
		
	}

	return boundRect;
}

/*//�޳�ֱ��
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
	Mat Temp = Mat::zeros(m_Size.height + 2, m_Size.width + 2, srcBw.type());//��չͼ��  
	srcBw.copyTo(Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)));

	cv::floodFill(Temp, Point(0, 0), Scalar(255));

	Mat cutImg;//�ü���չ��ͼ��  
	Temp(Range(1, m_Size.height + 1), Range(1, m_Size.width + 1)).copyTo(cutImg);

	Mat dstBw = srcBw | (~cutImg);
	return dstBw;
}

//�и�ͼƬ��Ϊ�˷ָ��
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

	//ȥ�����
	dilate(result, result, cv::Mat(), cv::Point(-1, -1), 4);

	//Canny(result, result, 100, 300, 3);
	vector <Mat> contours(500);

	findContours(result, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);//���������  

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



//�����ɫȱ��,��ֵ����ͼ����Ҫһ������ɫ

vector<Rect> check::detctColor(const char* path, int background){
	
	string reactFilePath ="E:\\��Ŀ\\ľ�ļ��\\�½��ļ���\\�����ļ�\\�����м�";
	string fileName=path;
	size_t t = fileName.find_last_of("\\");
	fileName = fileName.substr(t);
	reactFilePath = reactFilePath + fileName;
	clock_t a, b;
	a = clock();
	int bNumber=0;
	vector<Rect> newRects;
	//��ȡͼƬ
	cv::Mat image = cv::imread(path);
	
	if (image.empty()){
		cout << "image is empty\n";
		return newRects;
	}
	b = clock();
	cout << "clock:" << b - a <<"/"<< CLOCKS_PER_SEC<<"s\n";

	//��ȥ����
	//clearBackground(image, image);

	cv::Mat realImage = image;
	while (image.cols>10000 || image.rows>10000){
		resize(image, image, Size(image.cols / 2, image.rows / 2), 0, 0, INTER_LINEAR);
		bNumber++;
	}



	//ת��һ�³�Ϊ�Ҷ�ͼ��	
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, CV_BGR2GRAY);

	

	//��ֵ��ͼƬ
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
	
	//�������
	cv::Mat markers = getWaterMask(binaryImage);


	//��ͼƬ��ʼ��ˮ����
	cv::Mat waterImage = waterExcuteImage(markers, image);

	

	//ͨ����ˮ��ͼƬ���������������ͼƬ
	cv::Mat edgeImage = getClearEdge(waterImage);
	save(edgeImage, reactFilePath.c_str(), 3);

	//ͨ��������ͼƬ������������õ�����ʾ
	//Edge edgeTools;

	//���쳣����vector�޷��Զ�����
	//cv::vector<cv::vector<cv::Point>> contours; //= edgeTools.getContours(edgeImage)
	vector<Mat> contours(1000);//��Ҫ��Ϊ��������������ʵĵص㣬ʹ�õ�returnʱ�����vector�����ҵ���ȷ���ڴ�λ�ôӶ�����ȷ������
	cv::findContours(edgeImage, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	//end = clock();

	//cout << "findContours:" << end - start << "/" << CLOCKS_PER_SEC << " (s) " << endl;
	
	
	//CV_CHAIN_APPROX_NONE�洢���е������㣬���ڵ������������λ�ò����1����max��abs��x1-x2����abs��y2-y1����==1
	//CV_CHAIN_APPROX_SIMPLEѹ��ˮƽ���򣬴�ֱ���򣬶Խ��߷����Ԫ�أ�ֻ�����÷�����յ����꣬����һ����������ֻ��4����������������Ϣ
	//CV_CHAIN_APPROX_TC89_L1��CV_CHAIN_APPROX_TC89_KCOSʹ��teh-Chinl chain �����㷨

	//CV_RETR_EXTERNAL��ʾֻ���������
	//CV_RETR_TREE����һ���ȼ����ṹ������
	

	
	//���������Ӿ���ͬʱ����ЩϸС�ľ��ι���
	vector<Rect> rects = getBuddingRect(image, contours);
	
	cout << rects.size();
     rects=rectMerge::mergeRect(rects);


	cv::Mat rectImage = image.clone();

	
	for (int i = 0; i < rects.size(); i++){
		rectangle(rectImage, rects[i], Scalar(0, 0, 255), 1, 1, 0);//�þ��λ����δ�  
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







/*����Ĳ��õ��ã�����̭��*/
void check::detctColorErode(const char* path, int background){
	//��ȡͼƬ
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

	//ת��һ�³�Ϊ�Ҷ�ͼ��
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, CV_BGR2GRAY);
	//cv::imshow("GRAYImage", grayImage);


	//��ֵ��ͼƬ
	cv::Mat binaryImage = getBinaryImage(grayImage, background);


	threshold(grayImage, binaryImage, 0, 255, CV_THRESH_OTSU | CV_THRESH_BINARY_INV);
	
	cv::Mat edgeImage;
	Mat element = getStructuringElement(0, Size(8, 8));//MORPH_CROSS=1 MORPH_ELLIPSE=2 MORPH_RECT=0
	erode(binaryImage, edgeImage, element);
	save(edgeImage, path, 0);
	edgeImage = getClearEdge(edgeImage);
	

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
	//cout << rects.size() << endl;
	//rects = filterBuddingRect(rects);

	//cutting(image, rects, path);


	cv::Mat rectImage = image.clone();


	for (int i = 0; i < rects.size(); i++){
		rectangle(rectImage, rects[i], Scalar(0, 0, 255), 1, 1, 0);//�þ��λ����δ�  
	}

	//save(rectImage, path, 0);
	cout << path<<" cutover\n";
}
void check::detctColor(const char* path){

	//��ȡͼƬ
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

	//cutting(image, rects, newpath);


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
