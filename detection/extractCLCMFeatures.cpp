#include"extractGLCMFeatures.h"

using namespace extractGLCM;


//构造函数分别为距离，维数，方向
GLCM::GLCM(int dimension , GLCM_ANGLE direction ,int distance ){
		
	this->distance = distance;
	this->dimension = dimension;
	this->direction = direction;

	//动态分配了内存空间,需要析构函数将new消除
	this->comMatrix = new double*[dimension];
	for (int i = 0; i < dimension; i++){
		this->comMatrix[i] = new double[dimension];
	}

}

GLCM::~GLCM(){
	for (int i = 0; i < this->dimension; i++){
		delete[] this->comMatrix[i];
	}
	delete[] comMatrix;
}

void GLCM::getFeatures(const Mat& image,double* f){

	Mat grayImage = image.clone();
//	if (grayImage.channels() != 1){
//		cvtColor(grayImage, grayImage, CV_BGR2GRAY);
//	}

	this->getComMat(grayImage);
	this->calculateFeatures();

	//为了是的私有的不给外界改变的机会
	
	for (int i = 0; i < 14; i++){
		f[i] = this->features[i];
	}



}

void GLCM::calculateFeatures(){
	
	//初始化
	for (int i = 0; i < 14; i++){
		this->features[i] = 0;
	}

	//四个特征:E（能量）,I（惯性矩）,SOA（均值和）,SOE（聚类阴影）,SOV（方差和）
	double E=0, I=0, SOA=0, SOC=0, SOV=0;

	double ux = 0, uy = 0;


	for (int i = 0; i < this->dimension; i++){
		for (int j = 0; j < this->dimension; j++){
			E = E + this->comMatrix[i][j] * this->comMatrix[i][j];
			
			if (i - j >= 0 && i - j < this->dimension){
				I = I + (i - j)*(i - j)*this->comMatrix[j][i-j];
			}
			if (j - i >= 0 && j - i < this->dimension){
				I = I + (i - j)*(i - j)*this->comMatrix[j][j - i];
			}

			ux = ux + i*this->comMatrix[i][j];

			uy = uy + j*this->comMatrix[i][j];
		}
	}

	for (int k = 0; k <= 2 * (this->dimension - 1); k++){
		for (int i = 0; i <=k&&i<this->dimension; i++){
			if(k-i<this->dimension) SOA = SOA + k*this->comMatrix[i][k - i];
		}
		
	}

	for (int i = 0; i < this->dimension; i++){
		for (int j = 0; j < this->dimension; j++){
			SOC = SOC+((i - ux) + (j - uy))*((i - ux) + (j - uy))*this->comMatrix[i][j];
		}
	}
	SOC = -SOC;

	for (int k = 0; k <= 2 * (this->dimension - 1); k++){
		double px = 0;
		for (int i = 0; i <= k&&i<this->dimension; i++){
			if (k - i<this->dimension) px = px + this->comMatrix[i][k - i];
		}
		SOV = SOV + (k - SOA)*(k - SOA)*px;
	
	}

	this->features[0] = E;
	this->features[1] = I;
	this->features[2] = SOA;
	this->features[3] = SOC;//和SOV互为相反数？？？？？？？？？？？？？？？？？？？？？？？？？
	this->features[4] = SOV;
	/*for (int i = 0; i < 5; i++){
		cout << i << ":" << this->features[i] << endl;
	}
	*/
}

void GLCM::getComMat(cv::Mat& image){
	int a=0, b=1;//偏移量，同时a和b也确定了方向,(x,y)和(x+a,x+b)两点比较
	
	//确定的偏移量同时也确定了角度
	switch (this->direction){
	//水平方向
	case GLCM_ANGLE_HORIZATION:
		a = 0 * this->distance, b = 1 * this->distance;
		break;

	//垂直方向
	case GLCM_ANGLE_VERTICAL:
		a = 1 * this->distance, b = 0 * this->distance;
		break;

	//向下对角方向
	case GLCM_ANGLE_DIGONAL_45:
		a = 1 * this->distance, b = 1 * this->distance;
		break;

	//向上对角方向
	case GLCM_ANGLE_DIGONAL_135:
		a = -1 * this->distance, b = 1 * this->distance;
		break;
	default:
		break;
	}

	//对共生矩阵进行初始化
	for (int i = 0; i < this->dimension; i++){
		for (int j = 0; j < this->dimension; j++){
			this->comMatrix[i][j] = 0;
		}
	}

	//降维打击,将维数变化一下
	for (int i = 0; i < image.rows; i++){
		for (int j = 0; j < image.cols; j++){
			//cout << i << " " << j << ":" << (int)(image.at<uchar>(i, j)) << endl;
			//若将256维的变为x维,各个灰度直接的间隔t=256/x,对于灰度在256维上对应的灰度值为h的对应到x维上的值员工为h/t=h/(256/x)=h*x/256;
			image.at<uchar>(i, j) = uchar(1.0*image.at<uchar>(i, j)*this->dimension / 256);
		
			
		}
	}


	//观察降维后的矩阵
	cout << "imageMat：\n"<<image<<endl;
	


	double R = 0;//灰度共生矩阵的归一化常数

	//计算灰度共生矩阵
	for (int i = 0; i < image.rows; i++){
		for (int j = 0; j < image.cols; j++){

			int x = image.at<uchar>(i, j);
			
			//说明（x+a,y+b）这个点在图片上
			if (i + a >= 0 && i + a < image.cols&&j + b >= 0 && j + b < image.rows){
				int y = image.at<uchar>(i + a, j + b);

				this->comMatrix[x][y]++;
				this->comMatrix[y][x]++;//对于反方向的
			
				//cout << "<" << x << "," <<y<< ">" << endl;
				R++;
			}
		}
	}


	

	//灰度共生矩阵归一化
	for (int i = 0; i < this->dimension; i++){
		for (int j = 0; j < this->dimension; j++){
			this->comMatrix[i][j] = this->comMatrix[i][j] / R;
		}
	}

	//test测试灰度共生矩阵是不是正确
	cout <<"comMat:\n";
	for (int i = 0; i < this->dimension; i++){
		for (int j = 0; j < this->dimension; j++){
			cout << this->comMatrix[i][j] << " " ;
		}
		cout << endl;
	}

}