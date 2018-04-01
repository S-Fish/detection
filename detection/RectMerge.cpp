#include"RectMerge.h"


bool rectMerge::isOnRect(const Point&p, const Rect& rect){
	
	bool flag = false;
	Point a(rect.x, rect.y); 
	Point b(rect.x+rect.width, rect.y+rect.height);

	//是不是在两个与x平行的线段上
	if ( a.x==p.x  || p.x == b.x){
		if (p.y <= b.y&&p.y >= a.y){
			flag = true;
		}
	}else{
		if (p.y == a.y || p.y == b.y){
			if (p.x <= b.x&&p.x >= a.x){
				flag = true;
			}
		}
	}

	return flag;

}


bool rectMerge::isIntersect(const Rect& a, const Rect& b){

	bool flag = false;

	//a矩形的中心坐标
	float midAx = a.x + a.width / 2.0, midAy = a.y + a.height / 2.0;

	//b矩形的中心坐标
	float midBx = b.x + b.width / 2.0, midBy = b.y + b.height / 2.0;

	//计算x方向和y方向的距离
	float ABX = abs(midBx - midAx), ABY = abs(midBy - midAy);

	//用于比较的长度
	float width = (a.width + b.width) / 2.0, height = (a.height + b.height) / 2.0;

	//进行判断,若x和y方向分别都小于1/2长加高的话则说明两者相交（这里的相交包括包含关系）
	if (ABX <= width&&ABY <= height){
		flag = true;
	}


	return flag;
}


//之前使用判读相交的老方法
bool otherIsIntersect(const Rect& a, const Rect& b){
	bool flag = false;
	/*由于矩形a和矩形b是相互平行的，将矩形a和矩形b的边分别延长可以得到8个点，
	若是矩形a和矩形b完全不相干则这个8个点不会同时落在a和b上面.
	*/
	Point checkPoint[8];
	checkPoint[0] = Point(a.x, b.y);
	checkPoint[1] = Point(a.x, b.y + b.height);
	checkPoint[2] = Point(a.x + a.width, b.y);
	checkPoint[3] = Point(a.x + a.width, b.y + b.height);
	checkPoint[4] = Point(b.x, a.y);
	checkPoint[5] = Point(b.x, a.y + a.height);
	checkPoint[6] = Point(b.x + b.width, a.y);
	checkPoint[7] = Point(b.x + b.width, a.y + a.height);

	for (int i = 0; i < 8; i++){
		//判断点是不是既在a又在b上面,是的话则说明a和b相交
		if (rectMerge::isOnRect(checkPoint[i], a) && rectMerge::isOnRect(checkPoint[i], b)){
			flag = true;
			break;
		}
	}

	return flag;


}




//将矩形A和矩形b融合为一个大的矩形
Rect merge(const Rect& a, const Rect& b){
	int x1, y1, x2, y2;
	x1 = a.x < b.x ? a.x : b.x;
	y1 = a.y < b.y ? a.y : b.y;
	x2 = (a.x + a.width) >(b.x + b.width) ? (a.x + a.width) : (b.x + b.width);
	y2 = (a.y + a.height) >(b.y + b.height) ? (a.y + a.height) : (b.y + b.height);
	Rect tmp(Point(x1, y1), Point(x2, y2));
	return tmp;
}

//相交（包括包含的）的合并
vector<Rect> intersectMerge(vector<Rect>& rects){
	
	vector<Rect> result;
	bool flag = false;//判断是各个矩形之间是不是已经没有相交了，若是这可以退出递归
	bool ispushs[100];//判断第i个矩形是不是没有被合并了，被合并为false,没有合并为true
	
	//判断第i个矩形是不是被融合了
	for (int i = 0; i < 100; i++){
		ispushs[i] = true;
	}


	
	for (int i = 0; i < rects.size(); i++){//从第i个矩形开始
		
		//判断j之后的矩形 
		int j = i + 1;

		//判断条件的含义,如果矩形i已经被合并了就不需要继续，如果没有被合并就需要继续和后面的矩形进行判断
		while ( j < rects.size() &&ispushs[i]){
			/*
			debug分析使用的测试输出
			cv::Mat image(cv::Size(500, 800), CV_8UC3, cv::Scalar(255, 255, 255));
			rectangle(image, rects[i], Scalar(0, 0, 255), 1, 1, 0);//用矩形画矩形窗,红色
			rectangle(image, rects[j], Scalar(0, 255, 0), 1, 1, 0);//用矩形画矩形窗,绿色
			//rectangle(image, tmp, Scalar(255, 0, 0), 1, 1, 0);//用矩形画矩形窗  ，蓝色
			imshow("hechengdliangge矩形", image);
			waitKey(0);
			*/


			//先判断矩形j有没有之前就被合并过，才进行相交的判断
			if (ispushs[j] && rectMerge::isIntersect(rects[i], rects[j])){
				
				//说明这一步合成矩形，说明当前的rects会被更新，说明这个需要在进行相交判断
				flag = true;
				ispushs[i] = ispushs[j] = false;//说明i和j会被合成，所以i和j都不需要被保留下来

				//合并
				Rect tmp = merge(rects[i], rects[j]);

				//添加的结果中去
				result.push_back(tmp);
			}
			j++;
		}

		//说明没有经过上面while的if判断，说明i没有被合并所以保留下来
		if (ispushs[i]){
			result.push_back(rects[i]);
		}
		
	}
	if (flag){ 
		return intersectMerge(result); 
	}
	else
	{
		return result;
	}

}

vector<Rect> rectMerge::mergeRect(vector<Rect>& rects){
	
	//相交的合并
	vector<Rect> result = intersectMerge(rects);

	return result;

}