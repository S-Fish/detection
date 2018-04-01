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

	//a���ε���������
	float midAx = a.x + a.width / 2.0, midAy = a.y + a.height / 2.0;

	//b���ε���������
	float midBx = b.x + b.width / 2.0, midBy = b.y + b.height / 2.0;

	//����x�����y����ľ���
	float ABX = abs(midBx - midAx), ABY = abs(midBy - midAy);

	//���ڱȽϵĳ���
	float width = (a.width + b.width) / 2.0, height = (a.height + b.height) / 2.0;

	//�����ж�,��x��y����ֱ�С��1/2���ӸߵĻ���˵�������ཻ��������ཻ����������ϵ��
	if (ABX <= width&&ABY <= height){
		flag = true;
	}


	return flag;
}


//֮ǰʹ���ж��ཻ���Ϸ���
bool otherIsIntersect(const Rect& a, const Rect& b){
	bool flag = false;
	/*���ھ���a�;���b���໥ƽ�еģ�������a�;���b�ı߷ֱ��ӳ����Եõ�8���㣬
	���Ǿ���a�;���b��ȫ����������8���㲻��ͬʱ����a��b����.
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
		//�жϵ��ǲ��Ǽ���a����b����,�ǵĻ���˵��a��b�ཻ
		if (rectMerge::isOnRect(checkPoint[i], a) && rectMerge::isOnRect(checkPoint[i], b)){
			flag = true;
			break;
		}
	}

	return flag;


}




//������A�;���b�ں�Ϊһ����ľ���
Rect merge(const Rect& a, const Rect& b){
	int x1, y1, x2, y2;
	x1 = a.x < b.x ? a.x : b.x;
	y1 = a.y < b.y ? a.y : b.y;
	x2 = (a.x + a.width) >(b.x + b.width) ? (a.x + a.width) : (b.x + b.width);
	y2 = (a.y + a.height) >(b.y + b.height) ? (a.y + a.height) : (b.y + b.height);
	Rect tmp(Point(x1, y1), Point(x2, y2));
	return tmp;
}

//�ཻ�����������ģ��ĺϲ�
vector<Rect> intersectMerge(vector<Rect>& rects){
	
	vector<Rect> result;
	bool flag = false;//�ж��Ǹ�������֮���ǲ����Ѿ�û���ཻ�ˣ�����������˳��ݹ�
	bool ispushs[100];//�жϵ�i�������ǲ���û�б��ϲ��ˣ����ϲ�Ϊfalse,û�кϲ�Ϊtrue
	
	//�жϵ�i�������ǲ��Ǳ��ں���
	for (int i = 0; i < 100; i++){
		ispushs[i] = true;
	}


	
	for (int i = 0; i < rects.size(); i++){//�ӵ�i�����ο�ʼ
		
		//�ж�j֮��ľ��� 
		int j = i + 1;

		//�ж������ĺ���,�������i�Ѿ����ϲ��˾Ͳ���Ҫ���������û�б��ϲ�����Ҫ�����ͺ���ľ��ν����ж�
		while ( j < rects.size() &&ispushs[i]){
			/*
			debug����ʹ�õĲ������
			cv::Mat image(cv::Size(500, 800), CV_8UC3, cv::Scalar(255, 255, 255));
			rectangle(image, rects[i], Scalar(0, 0, 255), 1, 1, 0);//�þ��λ����δ�,��ɫ
			rectangle(image, rects[j], Scalar(0, 255, 0), 1, 1, 0);//�þ��λ����δ�,��ɫ
			//rectangle(image, tmp, Scalar(255, 0, 0), 1, 1, 0);//�þ��λ����δ�  ����ɫ
			imshow("hechengdliangge����", image);
			waitKey(0);
			*/


			//���жϾ���j��û��֮ǰ�ͱ��ϲ������Ž����ཻ���ж�
			if (ispushs[j] && rectMerge::isIntersect(rects[i], rects[j])){
				
				//˵����һ���ϳɾ��Σ�˵����ǰ��rects�ᱻ���£�˵�������Ҫ�ڽ����ཻ�ж�
				flag = true;
				ispushs[i] = ispushs[j] = false;//˵��i��j�ᱻ�ϳɣ�����i��j������Ҫ����������

				//�ϲ�
				Rect tmp = merge(rects[i], rects[j]);

				//��ӵĽ����ȥ
				result.push_back(tmp);
			}
			j++;
		}

		//˵��û�о�������while��if�жϣ�˵��iû�б��ϲ����Ա�������
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

	return result;

}