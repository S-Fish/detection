#ifndef RECTMERGE_H_
#define RECTMERGE_H_

#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<iostream>
using namespace cv;
using namespace std;


namespace rectMerge{

	//�жϵ�p�ǲ��Ǿ��α�����,���򷵻�true�������򷵻�false
	bool isOnRect(const Point&p, const Rect& rect);
	
	//�жϾ���A�;���B�Ƿ��ཻ,�ཻ����True�����ཻ����False
	bool isIntersect(const Rect& a,const Rect& b);

	//�жϾ���A�Ƿ��������B��A����B����True��A������B����Fasle
	bool isInclude(const Rect& a, const Rect& b);

	//��rects�ľ���ȫ���������,�������һ�����غϵľ���,Rects�������֮�䶼��ƽ�е�
	vector<Rect> mergeRect(vector<Rect>& rects);

}


#endif