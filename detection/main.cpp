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

//深度优先递归遍历当前目录下文件夹和文件及子文件夹和文夹，同时批量处理当前目录下面所有照片
void DfsFolder(string path)
{
	_finddata_t file_info;

	string current_path = path + "/*.*"; //也可以用/*来匹配所有  

	int handle = _findfirst(current_path.c_str(), &file_info);
	//返回值为-1则查找失败  
	if (-1 == handle)
	{
		cout << "cannot match the path" << endl;
		return;
	}

	do
	{
		//判断是否子目录  
		if (file_info.attrib == _A_SUBDIR)
		{
			//递归遍历子目录  

			//cout << file_info.name << endl;

			if (strcmp(file_info.name, "..") != 0 && strcmp(file_info.name, ".") != 0)  //.是当前目录，..是上层目录，必须排除掉这两种情况  
				DfsFolder(path + '/' + file_info.name); //再windows下可以用\\转义分隔符，不推荐  
		}
		else
		{
			//不是目录文件就需要访问
			//check::detctColorErode((path + '/' + file_info.name).c_str(), 19);
			check::detctColor((path + '/' + file_info.name).c_str(), 19);
			//cout << file_info.name << endl;
		}
	} while (!_findnext(handle, &file_info));  //返回0则遍历完  
	//关闭文件句柄  
	_findclose(handle);
}


int main(){
	
	char* path = "E:\\项目\\木材检测\\照片\\test\\D (19)\\2.jpg";
	
	
	//check::detctColor(path, 19);//（65,19），（89,4）
	DfsFolder("E:\\项目\\木材检测\\照片\\testRectErode");
	
	//cout << "test end" << endl;
	system("pause");


	return 0;
}



