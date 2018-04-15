#ifndef DATABASEMYSQL_H_
#define DATABASEMYSQL_H_

#include<winsock.h>
#include<mysql.h>
#include<iostream>
#include<vector>
#include<string>

using namespace std;

//特征的id
struct WoodFeature
{
	string id;
	vector<float> features;
};


//主要是用了c++的析构函数保护内存不被泄露，将result变为私有成员只能通过set改变，通过get获取
class SelectResult
{
private:
	MYSQL_RES* result = NULL;//用来存放结果的
public:
	

	SelectResult(MYSQL_RES* result);
	SelectResult();
	~SelectResult();
	MYSQL_RES* getResult();

	//判断result是不是为空，若先将result释放再将result赋值
	void setResult(MYSQL_RES* result);


};

//连接使用
class MySQLC
{
private:
	
	char host[200];
	char user[40];
	char password[40];
	char database[40];
	unsigned int port;
	bool state;			//判断连接状态，1表示mysqlCon连接成功，0表mysqlCon连接失败
	MYSQL* mysqlCon;

	bool connectMysql();


public:

	MySQLC(char* host, char* user, char* password,char* databaseName, unsigned int port);

	bool query(string sql);

	//获得连接，主要是其他的一些函数需要连接状态
	MYSQL* getConnect();

	//result需要手动释放
	MYSQL_RES* select(string sql);

	//result会自己释放
	bool select(string sql, SelectResult& res);
	
	~MySQLC();



};


//插入木材作用
class InsertWood{
private:
	string id;//插入的时候得到的木材id

public:

	//木材的长宽高，和木材照片的路径,返回被插入的木材id,若是没有输入id则直接例如自增增加的主键插入,插入成功则返回木材id否则返回""，
	//path可以不用管，这个是考虑了后面要是加入木材照片
	string insertWood(MySQLC& mysqlc, float length, float width, float height,string id="" ,string path = "");


	//插入一个woodId的木材的特征，如果为空则是插入上一个的木材id
	bool insertFeature(MySQLC& mysqlc, string featureName, const vector<float>& features,const string& woodId="");
	




	//这两个还没有实现
	//检测判断是不是有缺陷,若有则更新木材缺陷
	bool updateWood(MySQLC& mysqlc, bool isDefctive, const string& woodId = "");

	//插入缺陷
	bool insertDefects(MySQLC& mysqlc, const string& woodId = "");
	InsertWood();
	~InsertWood();


};


//从数据库里面读取数据
class  ReadWood
{

private:

public:
	 ReadWood();
	~ ReadWood();

	//获得所有的木材的featureName特征
	vector<WoodFeature> getAllWood(MySQLC& mysqlc, const string& featureName);
};



#endif