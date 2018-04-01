#include"extractGLCMFeatures.h"

using namespace extractGLCM;


//���캯���ֱ�Ϊ���룬ά��������
GLCM::GLCM(int dimension , GLCM_ANGLE direction ,int distance ){
		
	this->distance = distance;
	this->dimension = dimension;
	this->direction = direction;

	//��̬�������ڴ�ռ�,��Ҫ����������new����
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

	//Ϊ���ǵ�˽�еĲ������ı�Ļ���
	
	for (int i = 0; i < 14; i++){
		f[i] = this->features[i];
	}



}

void GLCM::calculateFeatures(){
	
	//��ʼ��
	for (int i = 0; i < 14; i++){
		this->features[i] = 0;
	}

	//�ĸ�����:E��������,I�����Ծأ�,SOA����ֵ�ͣ�,SOE��������Ӱ��,SOV������ͣ�
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
	this->features[3] = SOC;//��SOV��Ϊ�෴����������������������������������������������������
	this->features[4] = SOV;
	/*for (int i = 0; i < 5; i++){
		cout << i << ":" << this->features[i] << endl;
	}
	*/
}

void GLCM::getComMat(cv::Mat& image){
	int a=0, b=1;//ƫ������ͬʱa��bҲȷ���˷���,(x,y)��(x+a,x+b)����Ƚ�
	
	//ȷ����ƫ����ͬʱҲȷ���˽Ƕ�
	switch (this->direction){
	//ˮƽ����
	case GLCM_ANGLE_HORIZATION:
		a = 0 * this->distance, b = 1 * this->distance;
		break;

	//��ֱ����
	case GLCM_ANGLE_VERTICAL:
		a = 1 * this->distance, b = 0 * this->distance;
		break;

	//���¶ԽǷ���
	case GLCM_ANGLE_DIGONAL_45:
		a = 1 * this->distance, b = 1 * this->distance;
		break;

	//���϶ԽǷ���
	case GLCM_ANGLE_DIGONAL_135:
		a = -1 * this->distance, b = 1 * this->distance;
		break;
	default:
		break;
	}

	//�Թ���������г�ʼ��
	for (int i = 0; i < this->dimension; i++){
		for (int j = 0; j < this->dimension; j++){
			this->comMatrix[i][j] = 0;
		}
	}

	//��ά���,��ά���仯һ��
	for (int i = 0; i < image.rows; i++){
		for (int j = 0; j < image.cols; j++){
			//cout << i << " " << j << ":" << (int)(image.at<uchar>(i, j)) << endl;
			//����256ά�ı�Ϊxά,�����Ҷ�ֱ�ӵļ��t=256/x,���ڻҶ���256ά�϶�Ӧ�ĻҶ�ֵΪh�Ķ�Ӧ��xά�ϵ�ֵԱ��Ϊh/t=h/(256/x)=h*x/256;
			image.at<uchar>(i, j) = uchar(1.0*image.at<uchar>(i, j)*this->dimension / 256);
		
			
		}
	}


	//�۲콵ά��ľ���
	cout << "imageMat��\n"<<image<<endl;
	


	double R = 0;//�Ҷȹ�������Ĺ�һ������

	//����Ҷȹ�������
	for (int i = 0; i < image.rows; i++){
		for (int j = 0; j < image.cols; j++){

			int x = image.at<uchar>(i, j);
			
			//˵����x+a,y+b���������ͼƬ��
			if (i + a >= 0 && i + a < image.cols&&j + b >= 0 && j + b < image.rows){
				int y = image.at<uchar>(i + a, j + b);

				this->comMatrix[x][y]++;
				this->comMatrix[y][x]++;//���ڷ������
			
				//cout << "<" << x << "," <<y<< ">" << endl;
				R++;
			}
		}
	}


	

	//�Ҷȹ��������һ��
	for (int i = 0; i < this->dimension; i++){
		for (int j = 0; j < this->dimension; j++){
			this->comMatrix[i][j] = this->comMatrix[i][j] / R;
		}
	}

	//test���ԻҶȹ��������ǲ�����ȷ
	cout <<"comMat:\n";
	for (int i = 0; i < this->dimension; i++){
		for (int j = 0; j < this->dimension; j++){
			cout << this->comMatrix[i][j] << " " ;
		}
		cout << endl;
	}

}