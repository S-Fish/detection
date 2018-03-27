#ifndef RECTMERGE_H_
#define RECTMERGE_H_

#include<opencv2\core\core.hpp>
#include<opencv2\highgui\highgui.hpp>
#include<opencv2\imgproc\imgproc.hpp>
#include<iostream>
using namespace cv;
using namespace std;


namespace rectMerge{

	//判断点p是不是矩形边上面,是则返回true，不是则返回false
	bool isOnRect(const Point&p, const Rect& rect);
	
	//判断矩形A和矩形B是否相交,相交返回True，不相交返回False
	bool isIntersect(const Rect& a,const Rect& b);

	//判断矩形A是否包含矩形B，A包含B返回True，A不包含B返回Fasle
	bool isInclude(const Rect& a, const Rect& b);

	//将rects的矩形全部组合起来,结果返回一个不重合的矩形,Rects里面矩形之间都是平行的
	vector<Rect> mergeRect(vector<Rect>& rects);

}


#endif