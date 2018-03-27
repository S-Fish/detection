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
			//check::detctColorErode((path + '/' + file_info.name).c_str(), 19);
			check::detctColor((path + '/' + file_info.name).c_str(), 19);
			//cout << file_info.name << endl;
		}
	} while (!_findnext(handle, &file_info));  //����0�������  
	//�ر��ļ����  
	_findclose(handle);
}


int main(){
	
	char* path = "E:\\��Ŀ\\ľ�ļ��\\��Ƭ\\test\\D (19)\\2.jpg";
	
	
	//check::detctColor(path, 19);//��65,19������89,4��
	DfsFolder("E:\\��Ŀ\\ľ�ļ��\\��Ƭ\\testRectErode");
	
	//cout << "test end" << endl;
	system("pause");


	return 0;
}



