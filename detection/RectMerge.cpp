#include"RectMerge.h"


bool rectMerge::isOnRect(const Point&p, const Rect& rect){
	
	bool flag = false;
	Point a(rect.x, rect.y); 
	Point b(rect.x+rect.width, rect.y+rect.height);

	//�ǲ�����������xƽ�е��߶���
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
	/*���ھ���a�;���b���໥ƽ�еģ�������a�;���b�ı߷ֱ��ӳ����Եõ�8���㣬
	  ���Ǿ���a�;���b��ȫ����������8���㲻��ͬʱ����a��b����.
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
		//�жϵ��ǲ��Ǽ���a����b����,�ǵĻ���˵��a��b�ཻ
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
		rectangle(image, rects[i], Scalar(255), 1, 1, 0);//�þ��λ����δ�  
	}
	imshow("rect", image);
	cv::waitKey(0);
}


//�ཻ�ĺϲ�,��Ҫ�ع����߼���Ϊ����
vector<Rect> intersectMerge(vector<Rect>& rects){
	
	//paintRect(rects);
	vector<Rect> result;
	bool flag = false;//�ж��Ǹ�������֮���ǲ����Ѿ�û���ཻ�ˣ�����������˳��ݹ�
	bool ispushs[100];//�жϵ�i�������ǲ���û�б��ϲ��ˣ����ϲ�Ϊfalse�ϲ���Ϊtrue
	
	//�жϵ�i�������ǲ��Ǳ��ں���
	for (int i = 0; i < 100; i++){
		ispushs[i] = true;
	}


	
	for (int i = 0; i < rects.size(); i++){//�ӵ�i�����ο�ʼ
		
		
		//����i��������i+1�����һ�����ν��бȶ�,����i�������Ѿ��ϳ������i�����β���Ҫ�ټ�����
		for (int j = i + 1; j < rects.size() && ispushs[i]; j++){
			/*
			����i�;���j�ཻ��Ҫ����,������һ�ֲ��ܶ϶�result������û���ཻ��
			������Ҫ�ٶ�reslut���ж���ɸѡ
			*/
			/*
			cv::Mat image(cv::Size(500, 800), CV_8UC3, cv::Scalar(255, 255, 255));
			rectangle(image, rects[i], Scalar(0, 0, 255), 1, 1, 0);//�þ��λ����δ�,��ɫ
			rectangle(image, rects[j], Scalar(0, 255, 0), 1, 1, 0);//�þ��λ����δ�,��ɫ 
			//rectangle(image, tmp, Scalar(255, 0, 0), 1, 1, 0);//�þ��λ����δ�  ����ɫ
			imshow("hechengdliangge����", image);
			waitKey(0);
			*/
			if (ispushs[j]&& rectMerge::isIntersect(rects[i], rects[j])){
				flag = true;
				ispushs[i] = ispushs[j] = false;//˵��i��j�ᱻ�ϳɣ�����i��j������Ҫ����������
				//cout << "��"<<endl;
				int x1, y1, x2, y2;
				x1 = rects[i].x < rects[j].x ? rects[i].x : rects[j].x;
				y1 = rects[i].y < rects[j].y ? rects[i].y : rects[j].y;
				x2 = (rects[i].x + rects[i].width) > (rects[j].x + rects[j].width) ? (rects[i].x + rects[i].width) : (rects[j].x + rects[j].width);
				y2 = (rects[i].y + rects[i].height) >(rects[j].y + rects[j].height) ? (rects[i].y + rects[i].height) : (rects[j].y + rects[j].height);
				Rect tmp(Point(x1, y1), Point(x2, y2));
				
				/*
				cv::Mat image(cv::Size(500, 800), CV_8UC3, cv::Scalar(255,255,255));
				rectangle(image, rects[i], Scalar(0,0,255), 1, 1, 0);//�þ��λ����δ�,��ɫ
				rectangle(image, rects[j], Scalar(0,255,0), 1, 1, 0);//�þ��λ����δ�,��ɫ 
				rectangle(image, tmp, Scalar(255,0,0), 1, 1, 0);//�þ��λ����δ�  ����ɫ
				imshow("hechengdliangge����", image);
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
	
	//�ཻ�ĺϲ�
	vector<Rect> result = intersectMerge(rects);
	//��������ĺϲ�

	return result;

}