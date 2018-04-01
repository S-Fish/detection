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
		GLCM_ANGLE_HORIZATION,//ˮƽ���� ��1��0��
		GLCM_ANGLE_VERTICAL, //��ֱ����  ��0��1��
		GLCM_ANGLE_DIGONAL_45,//����45�� ��1��1��
		GLCM_ANGLE_DIGONAL_135//�����   ��-1��1��
	}GLCM_ANGLE;


	class GLCM
	{
	private:
		
		double features[14];//�����������ֵ
		
		GLCM_ANGLE direction;//����

		int distance;//���������ڵ���������֮�����С����

		int dimension;//ά��

		double** comMatrix;//��������

		void getComMat(cv::Mat& image);//���㹲������

		void calculateFeatures();//������������
	public:
		
		//Ĭ�ϵĲ�������Ϊ1��ά��Ϊ255������Ϊˮƽ����
		GLCM(int dimension = 256, GLCM_ANGLE direction = GLCM_ANGLE_HORIZATION,int distance = 1 );

		void getFeatures(const cv::Mat& images,double* f);//��ȡimages����������������

		~GLCM();

	};



}



#endif