#ifndef EXTRACTCLCMFEATURE_H_
#define EXTRACTCLCMFEATURE_H_


#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<iostream>
using namespace cv;
using namespace std;


namespace extractGLCM{

	typedef enum GLCM_ANGLE
	{
		GLCM_ANGLE_HORIZATION,//水平方向 （1，0）
		GLCM_ANGLE_VERTICAL, //垂直方向  （0，1）
		GLCM_ANGLE_DIGONAL_45,//向下45度 （1，1）
		GLCM_ANGLE_DIGONAL_135//向度上   （-1，1）
	}GLCM_ANGLE;


	class GLCM
	{
	private:
		
		double features[14];//保存各个特征值
		
		GLCM_ANGLE direction;//方向

		int distance;//倍数，相邻的两个坐标之间的最小距离

		int dimension;//维数

		double** comMatrix;//共生矩阵

		void getComMat(cv::Mat& image);//计算共生矩阵

		void calculateFeatures();//计算纹理特征
	public:
		
		//默认的参数距离为1，维度为255，方向为水平方向
		GLCM(int dimension = 256, GLCM_ANGLE direction = GLCM_ANGLE_HORIZATION,int distance = 1 );

		void getFeatures(const cv::Mat& images,double* f);//获取images的纹理共生矩阵特征

		~GLCM();

	};



}



#endif