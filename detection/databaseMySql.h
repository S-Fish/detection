#ifndef DATABASEMYSQL_H_
#define DATABASEMYSQL_H_

#include<winsock.h>
#include<mysql.h>
#include<iostream>

using namespace std;


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

	//result需要手动释放
	MYSQL_RES* select(string sql);

	//result会自己释放
	bool select(string sql, SelectResult& res);
	
	~MySQLC();



};



#endif