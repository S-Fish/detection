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
	/*由于矩形a和矩形b是相互平行的，将矩形a和矩形b的边分别延长可以得到8个点，
	  若是矩形a和矩形b完全不相干则这个8个点不会同时落在a和b上面.
	*/
	Point checkPoint[8];
	checkPoint[0] = Point(a.x, b.y);
	checkPoint[1] = Point(a.x, b.y+b.height);
	checkPoint[2] = Point(a.x+a.width, b.y);
	checkPoint[3] = Point(a.x+a.width, b.y+b.height);
	checkPoint[4] = Point(b.x, a.y);
	checkPoint[5] = Point(b.x, a.y+a.height);
	checkPoint[6] = Point(b.x+b.width, a.y);
	checkPoint[7] = Point(b.x+b.width, a.y+a.height);

	for (int i = 0; i < 8; i++){
		//判断点是不是既在a又在b上面,是的话则说明a和b相交
		if (isOnRect(checkPoint[i], a) && isOnRect(checkPoint[i], b)){
			flag = true;
			break;
		}
	}

	return flag;
}

bool rectMerge::isInclude(const Rect& a, const Rect &b);


void paintRect(const vector<Rect>& rects){
	cv::Mat image(cv::Size(500,800),CV_8U,cv::Scalar(0));

	for (int i = 0; i < rects.size(); i++){
		rectangle(image, rects[i], Scalar(255), 1, 1, 0);//用矩形画矩形窗  
	}
	imshow("rect", image);
	cv::waitKey(0);
}


//相交的合并,需要重构，逻辑较为不清
vector<Rect> intersectMerge(vector<Rect>& rects){
	
	//paintRect(rects);
	vector<Rect> result;
	bool flag = false;//判断是各个矩形之间是不是已经没有相交了，若是这可以退出递归
	bool ispushs[100];//判断第i个矩形是不是没有被合并了，被合并为false合并了为true
	
	//判断第i个矩形是不是被融合了
	for (int i = 0; i < 100; i++){
		ispushs[i] = true;
	}


	
	for (int i = 0; i < rects.size(); i++){//从第i个矩形开始
		
		
		//将第i个矩形与i+1到最后一个矩形进行比对,若第i个矩形已经合成了则第i个矩形不需要再继续了
		for (int j = i + 1; j < rects.size() && ispushs[i]; j++){
			/*
			矩形i和矩形j相交需要整合,所以这一轮不能断定result里面有没有相交的
			所以需要再对reslut进行二次筛选
			*/
			/*
			cv::Mat image(cv::Size(500, 800), CV_8UC3, cv::Scalar(255, 255, 255));
			rectangle(image, rects[i], Scalar(0, 0, 255), 1, 1, 0);//用矩形画矩形窗,红色
			rectangle(image, rects[j], Scalar(0, 255, 0), 1, 1, 0);//用矩形画矩形窗,绿色 
			//rectangle(image, tmp, Scalar(255, 0, 0), 1, 1, 0);//用矩形画矩形窗  ，蓝色
			imshow("hechengdliangge矩形", image);
			waitKey(0);
			*/
			if (ispushs[j]&& rectMerge::isIntersect(rects[i], rects[j])){
				flag = true;
				ispushs[i] = ispushs[j] = false;//说明i和j会被合成，所以i和j都不需要被保留下来
				//cout << "中"<<endl;
				int x1, y1, x2, y2;
				x1 = rects[i].x < rects[j].x ? rects[i].x : rects[j].x;
				y1 = rects[i].y < rects[j].y ? rects[i].y : rects[j].y;
				x2 = (rects[i].x + rects[i].width) > (rects[j].x + rects[j].width) ? (rects[i].x + rects[i].width) : (rects[j].x + rects[j].width);
				y2 = (rects[i].y + rects[i].height) >(rects[j].y + rects[j].height) ? (rects[i].y + rects[i].height) : (rects[j].y + rects[j].height);
				Rect tmp(Point(x1, y1), Point(x2, y2));
				
				/*
				cv::Mat image(cv::Size(500, 800), CV_8UC3, cv::Scalar(255,255,255));
				rectangle(image, rects[i], Scalar(0,0,255), 1, 1, 0);//用矩形画矩形窗,红色
				rectangle(image, rects[j], Scalar(0,255,0), 1, 1, 0);//用矩形画矩形窗,绿色 
				rectangle(image, tmp, Scalar(255,0,0), 1, 1, 0);//用矩形画矩形窗  ，蓝色
				imshow("hechengdliangge矩形", image);
				waitKey(0);
				*/
				result.push_back(tmp);
			}
		}
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
	//互相包括的合并

	return result;

}