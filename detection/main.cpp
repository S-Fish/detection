#include"histogram1D.h"
#include"decolor.h"
#include"extractGLCMFeatures.h"
#include"databaseMySql.h"
#include<io.h>
#include<string>
#include <iostream>  
#include <opencv2/core/core.hpp>  
#include <imgproc/imgproc.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/ml/ml.hpp>  
#include <stdio.h>
#include <windows.h>
#include<fstream>
#include"cluster.h"
typedef void(*ophandle)(const char*, int, int);
using namespace std;
using namespace extractGLCM;

string message = "";
int getThresh(cv::Mat image){
	int a = 1;
//	float coeffic[254];
	Histogram1D histogramTools;
	cv::Mat gaussImage;
	//GaussianBlur(image, gaussImage, cv::Size(5, 5), BORDER_DEFAULT);
	cv::Mat binary;
	//threshold(image, binary, 115, 255, THRESH_BINARY);
	int max = 0;
	cv::MatND hist = histogramTools.getHistogram(image);
	for (int i = 0; i < 256; i++){

		cout << i << ":" << hist.at<float>(i) << endl;
		if (hist.at<float>(i) > hist.at<float>(max)){
			max = i;
		}
	}

	cout << "max:" << max << endl;
	threshold(image, binary, 0, 255, THRESH_OTSU);
	cv::Mat histoImage = histogramTools.getHistogramImage(image);
	//cv::imshow("image", image);
	//cv::imshow("gaussImage", gaussImage);
	cv::imshow("histoimage", histoImage);
	//cv::imshow("binary", binary);
	//cv::waitKey(0);
	return a;

}

//深度优先递归遍历当前目录下文件夹和文件及子文件夹和文夹，同时批量处理当前目录下面所有照片
void DfsFolder(string path)
{
	_finddata_t file_info;

	string current_path = path + "/*.*"; //也可以用/*来匹配所有  

	int handle = _findfirst(current_path.c_str(), &file_info);
	//返回值为-1则查找失败  
	if (-1 == handle)
	{
		cout << "cannot match the path" << endl;
		return;
	}

	do
	{
		//判断是否子目录  
		if (file_info.attrib == _A_SUBDIR)
		{
			//递归遍历子目录  

			//cout << file_info.name << endl;

			if (strcmp(file_info.name, "..") != 0 && strcmp(file_info.name, ".") != 0)  //.是当前目录，..是上层目录，必须排除掉这两种情况  
				DfsFolder(path + '/' + file_info.name); //再windows下可以用\\转义分隔符，不推荐  
		}
		else
		{
			//不是目录文件就需要访问
			//check::detctColorErode((path + '/' + file_info.name).c_str(), 19);
			check::detctColor((path + '/' + file_info.name).c_str(), 19);
			//cout << file_info.name << endl;
		}
	} while (!_findnext(handle, &file_info));  //返回0则遍历完  
	//关闭文件句柄  
	_findclose(handle);
}



void testComMat(){
	GLCM glcm(4, GLCM_ANGLE_DIGONAL_135);//0，1，2，3
	uchar mat[6][6] = { {0,0,0,3,1,1},
						{0,0,0,0,1,1},
						{0,0,4,0,1,1},
						{0,0,0,0,1,1},
						{2,2,2,2,3,3}, 
						{2,2,2,2,3,3},

						//{1,2,2,1,0},

	};
	for (int i = 0; i < 6; i++){
		for (int j = 0; j < 6; j++){
			mat[i][j] = mat[i][j] * 64;
		}
	}


	Mat test(Size(6,6),CV_8UC1,mat);//Size(cols,rows)为rows行cols列
	//cvtColor(test, test, CV_BGR2GRAY);
	double f[14];
	glcm.getFeatures(test,f);
	for (int i = 0; i < 5; i++){
		cout << i << ":" << f[i] << endl;
	}
}


void testMySql(){

	MySQLC mysql("localhost","root","1234","wood_detection",3306);
	//string sql;

	/*
	//不使用SelectResult类手动释放MYSQL_RES
	sql = "insert t_teaching_material(id) values(5)";

	mysql.query(sql);
	sql = "delete from t_teaching_material where id=5";
	mysql.query(sql);
	

	MYSQL_RES* result;

	sql = "select * from t_teaching_material";

	result = mysql.select(sql);


	//各个字段（列名）
	unsigned int fieldcount = mysql_num_fields(result);
	MYSQL_FIELD *field = NULL;
	for (unsigned int i = 0; i < fieldcount; i++) {
		field = mysql_fetch_field_direct(result, i);
		cout << field->name << "\t\t";
	}

	//各行数据
	MYSQL_ROW row = NULL;
	row = mysql_fetch_row(result);
	while (NULL != row){
		for (int i = 0; i < fieldcount; i++){

			//MySql的NULL和空值不一样，NULL转化C++是一串空格，空值转化过来为NULL
			if(row[i]!=NULL) cout <<row[i] << "\t\t";
		}
		cout << endl;
		row = mysql_fetch_row(result);
	}


	if (result != NULL){
		mysql_free_result(result);
	}

	*/

	/*
	//使用SelectResult进行内存释放，自动释放MYSQL_RES
	cout << "sql1:" << endl;
	sql = "select * from t_teaching_material";

	SelectResult res;
	mysql.select(sql,res);
	cout << "sql2:" << endl;
	sql = "select* from t_roles";
	mysql.select(sql, res);

	cout << "end----------------" << endl;
	*/
	
	
	//插入
	InsertWood insert;

	vector<vector<float>> a;
	for (int i = 0; i < 10; i++){
		vector<float> t;
		t.push_back(i);
		t.push_back(1.2);
		t.push_back(i-0.1);
		t.push_back(i+2);
		a.push_back(t);
	}

	for (int i = 1; i <= 10; i++){
		insert.insertWood(mysql, 0, 0, 0);
		insert.insertFeature(mysql, "s", a.at(i-1));
	}
	

	
	//查询
	ReadWood readWood;

	vector<WoodFeature> S = readWood.getAllWood(mysql, "s");

	for (int i = 0; i < S.size(); i++){
		WoodFeature temp = S.at(i);
		cout << temp.id << ":";
		for (int j = 0; j < S.at(i).features.size(); j++){
			cout << S.at(i).features.at(j) << " ";
		}
		cout << endl;
	
	}
	

}


//读文件特征
void readfile(string& path){
	ifstream file(path);

	float a=0;
	int i = 0;
	while (!file.eof()){
		file >> a;
		cout <<i<<":"<< a <<endl;
		i++;
	}


	file.close();


}

//遍历文件
void DfsFolderGetfile(string path)
{
	_finddata_t file_info;

	string current_path = path + "/*.*"; //也可以用/*来匹配所有  

	int handle = _findfirst(current_path.c_str(), &file_info);
	//返回值为-1则查找失败  
	if (-1 == handle)
	{
		cout << "cannot match the path" << endl;
		return;
	}

	do
	{
		//判断是否子目录  
		if (file_info.attrib == _A_SUBDIR)
		{
			//递归遍历子目录  

			//cout << file_info.name << endl;

			if (strcmp(file_info.name, "..") != 0 && strcmp(file_info.name, ".") != 0)  //.是当前目录，..是上层目录，必须排除掉这两种情况  
				DfsFolder(path + '/' + file_info.name); //再windows下可以用\\转义分隔符，不推荐  
		}
		else
		{
			//不是目录文件就需要访问
			//check::detctColorErode((path + '/' + file_info.name).c_str(), 19);
			readfile(path + '/' + file_info.name);
			//cout << file_info.name << endl;
		}
	} while (!_findnext(handle, &file_info));  //返回0则遍历完  
	//关闭文件句柄  
	_findclose(handle);
}



int main(){
	
	char* path = "E:\\项目\\木材检测\\新建文件夹\\测试文件\\分析";
	
	
	//check::detctColor(path, 19);//（65,19），（89,4）
	DfsFolder(path);
	
	//cout << "test end" << endl;
	//testComMat();
	//testMySql();
	//string patht = "E:/项目/木材检测/新建文件夹/特征值/1.txt";
	//readfile(patht);
	
	/*
	char * test = "E:\\项目\\木材检测\\照片\\IMAGE\\A (18)\\1test.jpg";
	cv::Mat image;
	image = cv::imread(test,0);
	cv::Mat result;
	cluster::pretreat(image, result);
	cv::threshold(result, result, 0, 255, THRESH_OTSU);
	cv::imshow("test",result);
	cv::threshold(image, image, 0, 255, THRESH_OTSU);
	cv::imshow("image", image);
	cv::waitKey(0);
	system("pause");
	*/
	string fileName = path;
	fileName = fileName + "\\result.txt";

	ofstream os(fileName);
	os << message;
	os.close();

	system("pause");
	return 0;
}



