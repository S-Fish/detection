#include"histogram1D.h"
#include"decolor.h"
#include<io.h>
#include<string>
#include <iostream>  
#include <opencv2/core/core.hpp>  
#include <imgproc/imgproc.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/ml/ml.hpp>  
#include <stdio.h>
#include <windows.h>

typedef void(*ophandle)(const char*, int, int);
using namespace std;


int getThresh(cv::Mat image){
	int a = 1;
	float coeffic[254];
	Histogram1D histogramTools;
	cv::Mat gaussImage;
	//GaussianBlur(image, gaussImage, cv::Size(5, 5), BORDER_DEFAULT);
	cv::Mat binary;
	//threshold(image, binary, 115, 255, THRESH_BINARY);
	int max = 0;
	cv::MatND hist = histogramTools.getHistogram(image);
	for (int i = 0; i < 256; i++){

		cout << i << ":" << hist.at<float>(i) << endl;
		if (hist.at<float>(i) > hist.at<float>(max)){
			max = i;
		}
	}

	cout << "max:" << max << endl;
	threshold(image, binary, 0, 255, THRESH_OTSU);
	cv::Mat histoImage = histogramTools.getHistogramImage(image);
	//cv::imshow("image", image);
	//cv::imshow("gaussImage", gaussImage);
	cv::imshow("histoimage", histoImage);
	//cv::imshow("binary", binary);
	//cv::waitKey(0);
	return a;

}

//������ȵݹ������ǰĿ¼���ļ��к��ļ������ļ��к��ļУ�ͬʱ��������ǰĿ¼����������Ƭ
void DfsFolder(string path)
{
	_finddata_t file_info;

	string current_path = path + "/*.*"; //Ҳ������/*��ƥ������  

	int handle = _findfirst(current_path.c_str(), &file_info);
	//����ֵΪ-1�����ʧ��  
	if (-1 == handle)
	{
		cout << "cannot match the path" << endl;
		return;
	}

	do
	{
		//�ж��Ƿ���Ŀ¼  
		if (file_info.attrib == _A_SUBDIR)
		{
			//�ݹ������Ŀ¼  

			//cout << file_info.name << endl;

			if (strcmp(file_info.name, "..") != 0 && strcmp(file_info.name, ".") != 0)  //.�ǵ�ǰĿ¼��..���ϲ�Ŀ¼�������ų������������  
				DfsFolder(path + '/' + file_info.name); //��windows�¿�����\\ת��ָ��������Ƽ�  
		}
		else
		{
			//����Ŀ¼�ļ�����Ҫ����

			check::detctColor((path + '/' + file_info.name).c_str(), 19);
			cout << file_info.name << endl;
		}
	} while (!_findnext(handle, &file_info));  //����0�������  
	//�ر��ļ����  
	_findclose(handle);
}


void DfsFolder(string hsource, string htarget, string tail)
{
	_finddata_t file_info;

	string current_path = hsource + tail + "/*.*"; //Ҳ������/*��ƥ������  

	int handle = _findfirst(current_path.c_str(), &file_info);
	//����ֵΪ-1�����ʧ��  
	if (-1 == handle)
	{
		cout << "cannot match the path" << endl;
		return;
	}

	do
	{
		//�ж��Ƿ���Ŀ¼  
		if (file_info.attrib == _A_SUBDIR)
		{
			//�ݹ������Ŀ¼  

			cout << file_info.name << endl;

			if (strcmp(file_info.name, "..") != 0 && strcmp(file_info.name, ".") != 0)  //.�ǵ�ǰĿ¼��..���ϲ�Ŀ¼�������ų������������  
				//DfsFolder(path + '/' + file_info.name); //��windows�¿�����\\ת��ָ��������Ƽ�  
				DfsFolder(hsource, htarget, tail + '/' + file_info.name);
		}
		else
		{
			//����Ŀ¼�ļ�����Ҫ����
			check::detctColor(hsource.c_str(), htarget.c_str(), (tail + '/' + file_info.name).c_str(), 75, 40);
			//test(hsource, htarget, tail);
			cout << file_info.name << endl;
		}
	} while (!_findnext(handle, &file_info));  //����0�������  
	//�ر��ļ����  
	_findclose(handle);
}

void dt(){

	string head = "E:\\��Ŀ\\ľ�ļ��\\��Ƭ\\IMAGE\\test\\";
	int i, j;
	for (i = 1; i <= 14; i++){
		for (j = 1; j <= 5; j++){
			char tail[100];
			sprintf(tail, "A (%d)\\%d.jpg", i, j);
			string path = "E:\\��Ŀ\\ľ�ļ��\\��Ƭ\\test\\";
			path.append(tail);
			cout << path << endl;
			check::detctColor(path.c_str(), 75);
			//cout << "yyyy" << endl;

		}
	}

}

string WChar2Ansi(LPCWSTR pwszSrc)
{
	int nLen = WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, NULL, 0, NULL, NULL);

	if (nLen <= 0) return string("");

	char* pszDst = new char[nLen];
	if (NULL == pszDst) return string("");

	WideCharToMultiByte(CP_ACP, 0, pwszSrc, -1, pszDst, nLen, NULL, NULL);
	pszDst[nLen - 1] = 0;

	string strTemp(pszDst);
	delete[] pszDst;

	return strTemp;
}

void clusterBinaryImage(cv::Mat &inImage, cv::Mat& outImage, int clusterCount){

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

int main(){
	
	char* path = "E:\\��Ŀ\\ľ�ļ��\\��Ƭ\\test\\D (19)\\2.jpg";
	
	
	//check::detctColor(path, 19);//��65,19������89,4��
	DfsFolder("E:\\��Ŀ\\ľ�ļ��\\��Ƭ\\debug");
	
	//cout << "test end" << endl;
	//system("pause");


	return 0;
}



